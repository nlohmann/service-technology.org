package prom;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.deckfour.xes.classification.XEventClass;
import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.in.XMxmlParser;
import org.deckfour.xes.in.XParser;
import org.deckfour.xes.in.XParserRegistry;
import org.deckfour.xes.in.XesXmlParser;
import org.deckfour.xes.info.XLogInfo;
import org.deckfour.xes.info.XLogInfoFactory;
import org.deckfour.xes.info.impl.XLogInfoImpl;
import org.deckfour.xes.model.XLog;
import org.processmining.framework.plugin.PluginContext;
import org.processmining.models.connections.GraphLayoutConnection;
import org.processmining.models.graphbased.directed.opennet.OpenNet;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.astar.petrinet.*;
import org.processmining.plugins.connectionfactories.logpetrinet.TransEvClassMapping;
import org.processmining.plugins.petrinet.replayer.PNLogReplayer;
import org.processmining.plugins.petrinet.replayer.algorithms.costbasedcomplete.CostBasedCompleteParam;
import org.processmining.plugins.petrinet.replayresult.PNRepResult;
import org.processmining.plugins.petrinet.replayresult.StepTypes;
import org.processmining.plugins.pnml.Pnml;
import org.processmining.plugins.pnml.importing.PnmlImportUtils;
import org.processmining.plugins.replayer.replayresult.SyncReplayResult;

import prom.ExportPNRepResultChanged;


public class consoleReplayer{
	
	
	public static void main(String[] args) {
		System.out.println("Arguments sent to this program:");
		if (args.length == 0) {
			System.out.println("(None)");
		} else {
			for (int i=0; i<args.length; i++) {
				System.out.print(args[i] + " ");
			}
			System.out.println();
		}
		
//		String path = "C:\\Users\\SimHei\\Documents\\";
//		String pnmlFilename = "1.private.sync.pnml";
//		String logFilename = "1.private.xes";
		
		useReplayer(args[0], args[0], args[1], args[1]);
	}
	
	public static Object myImportPnmlFromStream(PluginContext context, InputStream input, String filename, long fileSizeInBytes)
			throws Exception {		
		PnmlImportUtils utils = new PnmlImportUtils();
		Pnml pnml = utils.importPnmlFromStream(context, input, filename, fileSizeInBytes);
		if (pnml == null) {
			/*
			 * No PNML found in file. Fail.
			 */
			return null;
		}
		
		/*
		 * PNML file has been imported. Now we need to convert the contents to a
		 * regular Petri net.
		 */
		OpenNet openNet = new OpenNet(pnml.getLabel() + " (imported from " + filename + ")");
		Marking initMarking = new Marking();
		
		GraphLayoutConnection layout = new GraphLayoutConnection(openNet);
		pnml.convertToNet(openNet, initMarking, layout);
		
		/*
		 * Return the net and the marking.
		 */
		Object[] objects = new Object[2];
		objects[0] = openNet;
		objects[1] = initMarking;
		return objects;
	}

	public static XLog myImportLogFromStream(PluginContext context, InputStream input, String filename, long fileSizeInBytes, XFactory factory)
			throws Exception {
//		context.getFutureResult(0).setLabel(filename);
		//	System.out.println("Open file");
		XParser parser;
		if (filename.toLowerCase().endsWith(".xes") || filename.toLowerCase().endsWith(".xez")
				|| filename.toLowerCase().endsWith(".xes.gz")) {
			parser = new XesXmlParser(factory);
		} else {
			parser = new XMxmlParser(factory);
		}
		Collection<XLog> logs = null;
		try {
			//logs = parser.parse(new XContextMonitoredInputStream(input, fileSizeInBytes, context.getProgress()));
			logs = parser.parse(input);
		} catch (Exception e) {
			logs = null;
		}
		if (logs == null) {
			// try any other parser
			for (XParser p : XParserRegistry.instance().getAvailable()) {
				if (p == parser) {
					continue;
				}
				try {
					//logs = p.parse(new XContextMonitoredInputStream(input, fileSizeInBytes, context.getProgress()));
					logs = p.parse(input);
					if (logs.size() > 0) {
						break;
					}
				} catch (Exception e1) {
					// ignore and move on.
					logs = null;
				}
			}
		}

		// log sanity checks;
		// notify user if the log is awkward / does miss crucial information
		if (logs == null || logs.size() == 0) {
			throw new Exception("No processes contained in log!");
		}

		XLog log = logs.iterator().next();
		if (XConceptExtension.instance().extractName(log) == null) {
			/*
			 * Log name not set. Create a default log name.
			 */
			XConceptExtension.instance().assignName(log, "Anonymous log imported from " + filename);
		}

		if (log.isEmpty()) {
			throw new Exception("No process instances contained in log!");
		}

//		/*
//		 * Set the log name as the name of the provided object.
//		 */
//		if (context != null) {
//			context.getFutureResult(0).setLabel(XConceptExtension.instance().extractName(log));
//		}

		return log;

	}
	
	public static void useReplayer(String pnmlFilename, String pnmlFilePath, String logFilename, String logFilePath) {
		File pnmlFile = new File(pnmlFilePath);
		File logFile = new File(logFilePath);
		
		Object[] netAndMarking = null;
		try {
			netAndMarking = (Object[]) myImportPnmlFromStream(null, new FileInputStream(pnmlFile), pnmlFilename, pnmlFile.length());
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		OpenNet openNet = (OpenNet)netAndMarking[0];
		Marking initMarking = (Marking)netAndMarking[1];
		
		System.out.println();
		System.out.print("init marking: \t");
		System.out.println(initMarking.toString());
		System.out.print("places: \t");
		System.out.println(openNet.getPlaces().toString());
		System.out.print("transitions: \t");
		System.out.println(openNet.getTransitions().toString());
		System.out.print("final markings:\t");
		System.out.println(openNet.getFinalMarkings().toString());
		System.out.println();
		
		XLog log = null;
		XFactory factory = XFactoryRegistry.instance().currentDefault();
		try {
			log = myImportLogFromStream(null, new FileInputStream(logFile), logFilename, logFile.length(), factory);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		//System.out.println(log.get(0).get(0).getAttributes().toString());
		
		XLogInfo logInfo = XLogInfoFactory.createLogInfo(log);
		
		System.out.print("log events: \t");
		System.out.println(logInfo.getEventClasses().getClasses().toString());
		
		XEventClass evClassDummy = new XEventClass("DUMMY", -1);
		
		boolean foundMapping = false;
		// create mapping
		TransEvClassMapping mapping = new TransEvClassMapping(XLogInfoImpl.STANDARD_CLASSIFIER, evClassDummy);
		for (Transition t : openNet.getTransitions()) {
			foundMapping = false;
			for (XEventClass ec : logInfo.getEventClasses().getClasses()) {
				if (ec.toString().equals(t.getLabel().concat("+complete"))) {
					mapping.put(t, ec);
					foundMapping = true;
					//System.out.print("1");
					continue;
				}
			}
			if (!foundMapping) {
				mapping.put(t, evClassDummy);
				foundMapping = false;
				//System.out.print("2");
			}
		}
		
		System.out.print("mapping: \t");
		System.out.println(mapping.toString());
		
		//now do some stuff with the replayer, damn yeah!
		
		// create parameter
		CostBasedCompleteParam parameter = new CostBasedCompleteParam(logInfo.getEventClasses().getClasses(),
				evClassDummy, openNet.getTransitions(), 2, 5);
		parameter.setGUIMode(false);
		parameter.setCreateConn(false);

		parameter.setInitialMarking(initMarking);

		parameter.setFinalMarkings(openNet.getFinalMarkings().toArray(new Marking[0]));
		parameter.setMaxNumOfStates(200000);

		// instantiate replayer
		PNLogReplayer replayer = new PNLogReplayer();

		System.out.println();
		
		// select algorithm without ILP
		PetrinetReplayerWithoutILP replWithoutILP = new PetrinetReplayerWithoutILP();
		PNRepResult pnRepResult = replayer.replayLog(null, openNet, log, mapping, replWithoutILP, parameter);
		System.out.println("algorithm without ILP:");
		validateResult(pnRepResult);

		// select algorithm SSD
		PetrinetReplayerSSD replSSD = new PetrinetReplayerSSD();
		PNRepResult pnRepResult2 = replayer.replayLog(null, openNet, log, mapping, replSSD, parameter);
		System.out.println("algorithm SSD:");
		validateResult(pnRepResult2);
		
		// select algorithm prefix based
		PrefixBasedPetrinetReplayer replPrefix = new PrefixBasedPetrinetReplayer();
		PNRepResult pnRepResult3 = replayer.replayLog(null, openNet, log, mapping, replPrefix, parameter);
		System.out.println("algorithm prefix based:");
		validateResult(pnRepResult3);
		
		ExportPNRepResultChanged exporter = new ExportPNRepResultChanged();
		String filename = logFilePath.concat(".csv");
		File file = new File(filename);
		try {
			exporter.exportRepResult2File(pnRepResult3, file, openNet, log);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private static void validateResult(PNRepResult pnRepResult) {
		for (SyncReplayResult syncRepResult : pnRepResult){
			System.out.print(syncRepResult.getTraceIndex().toString().concat(": "));
			
			assert(syncRepResult.isReliable()); // result should be reliable
			Map<String, Double> info = syncRepResult.getInfo();			
			assert(Double.compare(7.000, info.get(PNRepResult.RAWFITNESSCOST)) == 0); // one move log, one move model
			
			List<Object> nodeInstance = syncRepResult.getNodeInstance();
			List<StepTypes> stepTypes = syncRepResult.getStepTypes();
			
			Iterator<Object> it = nodeInstance.iterator();
			for (StepTypes st : stepTypes){
				switch(st){
					case LMGOOD: 	//step in the log and the model
						//it.next();
						System.out.print(((Transition)it.next()).getLabel());
						System.out.print("(L/M)");
						break;
					case L:			//step in the log only
						//assert(it.next().toString().equals("A+complete"));
						System.out.print(it.next());
						System.out.print("(L)");
						break;
					case MREAL:		//step in the model only
						//assert(((Transition)it.next()).getLabel().equals("C"));
						System.out.print(((Transition)it.next()).getLabel());
						System.out.print("(M)");
						break;
					default:
						assert(false);
				}
			}
			System.out.println();
		}
		System.out.println();
	}
}
