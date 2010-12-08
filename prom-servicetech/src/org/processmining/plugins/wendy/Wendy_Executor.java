package org.processmining.plugins.wendy;

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
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.configuration.Configuration;
import org.processmining.plugins.configuration.ConfigurationStream;
import org.processmining.plugins.pnml.Pnml;

/**
 * Wrapper for the execution of Wendy.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @since March 2010
 * @version March 2010
 */
public class Wendy_Executor {
	/// which path to prefix to system calls for Wendy
	private static String wendyPath;

	/// which path to prefix to system calls for LoLA
	private static String lolaPath;

	/// whether the plugin has been initialized
	private static boolean initialized = false;

	// Maps the exported ids to the corresponding object (place or transition).
	private static Map<String, AbstractGraphElement> idMap;

	private final int mode;

	public static AbstractGraphElement getNode(String name) {
		return idMap.get(name);
	}

	Wendy_Executor(int mode) {
		this.mode = mode;
	}

	private static synchronized void initialize(PluginContext context) {
		if (initialized) {
			return;
		}
		PackageManager manager = PackageManager.getInstance();

		try {
			PackageDescriptor[] packages = null;
			packages = manager.findOrInstallPackages("Wendy", "LoLA");
			wendyPath = packages[0].getLocalPackageDirectory().getAbsolutePath() + File.separator;
			lolaPath = packages[1].getLocalPackageDirectory().getPath() + File.separator;
			initialized = true;
		} catch (Exception e) {
			wendyPath = "";
			lolaPath = "";
			initialized = false;
			context.log(e);
		}
	}

	// do the actual work: convert PNML to LoLA, call LoLA and return result
	public Configuration executeWendy(PluginContext context, Petrinet net, Marking m) throws IOException,
			InterruptedException {
		initialize(context);

		if (!initialized) {
			throw new InterruptedException("Wendy Initialization failed.");
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
		File outputFilePNML = File.createTempFile("wendy", ".pnml");
		File outputFileResults = File.createTempFile("wendy", ".results");
		outputFilePNML.deleteOnExit();
		outputFileResults.deleteOnExit();

		// write the PNML file
		String netPnmlText = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" + netPnml.exportElement(netPnml);
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(outputFilePNML)));
		bw.write(netPnmlText);
		bw.close();

		// call Wendy and wait until it is finished
		ProcessBuilder b = null;
		switch (mode) {
			case (0) :
				b = new ProcessBuilder(wendyPath + "wendy", outputFilePNML.getAbsolutePath(), "--resultFile="
						+ outputFileResults.getAbsolutePath(), "--pnml", "--lola=" + lolaPath + "lola-statespace",
						"--og");
				break;
			case (1) :
				b = new ProcessBuilder(wendyPath + "wendy", outputFilePNML.getAbsolutePath(), "--resultFile="
						+ outputFileResults.getAbsolutePath(), "--pnml", "--lola=" + lolaPath + "lola-statespace",
						"--sa");
				break;
			case (2) :
				b = new ProcessBuilder(wendyPath + "wendy", outputFilePNML.getAbsolutePath(), "--resultFile="
						+ outputFileResults.getAbsolutePath(), "--pnml", "--lola=" + lolaPath + "lola-statespace",
						"--sa", "--receivingBeforeSending", "--waitstatesOnly", "--quitAsSoonAsPossible");
				break;
		}

		Process processWendy = b.start();
		processWendy.getOutputStream().close();
		processWendy.getErrorStream().close();
		ct.setProcess(processWendy);

		processWendy.waitFor();
		if (context.getProgress().isCancelled()) {
			throw new InterruptedException("Execution cancelled by user");
		}

		// read Wendy's output and create a configuration file
		InputStream input = new FileInputStream(outputFileResults.getAbsoluteFile());
		Configuration cfg = new Configuration();
		cfg.importElement(new ConfigurationStream(input));
		input.close();

		return cfg;
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