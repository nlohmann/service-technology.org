package org.processmining.plugins.lola;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.Map;

import org.processmining.framework.packages.PackageDescriptor;
import org.processmining.framework.packages.PackageManager;
import org.processmining.framework.plugin.PluginContext;
import org.processmining.framework.plugin.Progress;
import org.processmining.models.graphbased.AbstractGraphElement;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.semantics.petrinet.CTMarking;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.configuration.Configuration;
import org.processmining.plugins.configuration.ConfigurationList;
import org.processmining.plugins.configuration.ConfigurationStream;
import org.processmining.plugins.pnml.Pnml;

/**
 * Wrapper for the execution of LoLA.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @since March 2010
 * @version March 2010
 */
public enum LoLA_Executor {
	BOUNDEDNET("lola-prom-boundednet"), DEADLOCK("lola-prom-deadlock"), HOME("lola-prom-home"), REVERSIBILITY(
			"lola-prom-reversibility");

	/// which path to prefix to system calls for LoLA
	private static String lolaPath;

	/// which path to prefix to system calls for PNAPI
	private static String petriPath;

	/// the name of the executable to call
	private final String name;

	/// whether the plugin has been initialized
	private static boolean initialized = false;

	// Maps the exported ids to the corresponding object (place or transition).
	private static Map<String, AbstractGraphElement> idMap;

	public static AbstractGraphElement getNode(String name) {
		return idMap.get(name);
	}

	private LoLA_Executor(String name) {
		this.name = name;
	}

	private static synchronized void initialize(PluginContext context) {
		if (initialized) {
			return;
		}
		PackageManager manager = PackageManager.getInstance();

		try {
			PackageDescriptor[] packages = null;
			packages = manager.findOrInstallPackages("Petri Net API", "LoLA");
			petriPath = packages[0].getLocalPackageDirectory().getAbsolutePath() + File.separator;
			lolaPath = packages[1].getLocalPackageDirectory().getAbsolutePath() + File.separator;
			initialized = true;
		} catch (Exception e) {
			lolaPath = "";
			petriPath = "";
			initialized = false;
			context.log(e);
		}
	}

	// do the actual work: convert PNML to LoLA, call LoLA and return result
	public Configuration executeLoLA(PluginContext context, Petrinet net, Marking m) throws IOException,
			InterruptedException {
		initialize(context);

		if (!initialized) {
			throw new InterruptedException("LoLA Initialization failed.");
		}

		idMap = new HashMap<String, AbstractGraphElement>();
		Pnml netPnml = new Pnml().convertFromNet(net, m, idMap);
		netPnml.setType(Pnml.PnmlType.LOLA);
		/*
		 * idMap now contains the mapping from exported ids to
		 * places/transitions
		 */

		CancellationThread ct = new CancellationThread(context.getProgress());

		// create some temp files
		File outputFilePNML = File.createTempFile("lola", ".pnml");
		File outputFileResults = File.createTempFile("lola", ".results");
		outputFilePNML.deleteOnExit();
		outputFileResults.deleteOnExit();

		// write the PNML file
		String netPnmlText = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" + netPnml.exportElement(netPnml);
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(outputFilePNML)));
		bw.write(netPnmlText);
		bw.close();

		// call Petri Net API to translate and wait until it is finished
		ProcessBuilder b = new ProcessBuilder(petriPath + "petri", outputFilePNML.getAbsolutePath(), "-ipnml", "-olola");

		Process processPetri = b.start();
		processPetri.getInputStream().close();
		processPetri.getErrorStream().close();
		ct.setProcess(processPetri);
		processPetri.waitFor();
		if (context.getProgress().isCancelled()) {
			throw new InterruptedException("Execution cancelled by user");
		}
		File outputFileLoLA = new File(outputFilePNML.getAbsolutePath() + ".lola");
		outputFileLoLA.deleteOnExit();

		// call LoLA and wait until it is finished
		b = new ProcessBuilder(lolaPath + name, outputFileLoLA.getAbsolutePath(), "--resultFile="
				+ outputFileResults.getAbsolutePath());

		Process processLoLA = b.start();
		processLoLA.getOutputStream().close();
		processLoLA.getErrorStream().close();
		ct.setProcess(processLoLA);

		processLoLA.waitFor();
		if (context.getProgress().isCancelled()) {
			throw new InterruptedException("Execution cancelled by user");
		}

		// read LoLA's output and create a configuration file
		InputStream input = new FileInputStream(outputFileResults.getAbsoluteFile());
		Configuration cfg = new Configuration();
		cfg.importElement(new ConfigurationStream(input));
		input.close();

		return cfg;
	}

	// returns a marking object from an entry in a results file
	public static CTMarking constructMarking(Petrinet net, ConfigurationList state) {
		CTMarking marking = new CTMarking();
		for (int placeCtr = 0; placeCtr < state.size(); placeCtr++) {
			String placeId = state.getValue(placeCtr + ".0", null);
			Integer weight = state.getValue(placeCtr + ".1", 0);
			if ((placeId != null) && (weight != 0)) {
				if (idMap.keySet().contains(placeId)) {
					AbstractGraphElement n = idMap.get(placeId);
					if (n instanceof Place) {
						if (weight == -1) {
							marking.addOmegaPlace((Place) n);
							marking.add((Place) n, 1);
						} else {
							marking.add((Place) n, weight);
						}
					}
				}
			}
		}
		return marking;
	}
}

class CancellationThread {

	private Thread thread;

	private final Progress progress;

	// Start cancellation thread
	public CancellationThread(Progress progress) {
		this.progress = progress;
	}

	public void setProcess(final Process process) {
		thread = new Thread(new Runnable() {

			public void run() {
				while (true) {
					// If canceled, destroy process
					if (progress.isCancelled()) {
						process.destroy();
					}
					// Check if process finished. Has to be done with exception 
					// handling
					try {
						process.exitValue();
						return;
					} catch (IllegalThreadStateException e) {
						// Not finished yet,
					}
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
					}
				}
			}

		});
		thread.start();

	}
}
