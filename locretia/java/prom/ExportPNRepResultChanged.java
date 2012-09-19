/**
 * 
 */
package prom;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.text.NumberFormat;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.model.XLog;
import org.processmining.contexts.uitopia.UIPluginContext;
import org.processmining.contexts.uitopia.annotations.UIExportPlugin;
import org.processmining.framework.plugin.annotations.Plugin;
import org.processmining.framework.plugin.annotations.PluginVariant;
import org.processmining.models.connections.petrinets.PNRepResultAllRequiredParamConnection;
import org.processmining.models.connections.petrinets.PNRepResultConnection;
import org.processmining.models.graphbased.directed.petrinet.PetrinetGraph;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.plugins.petrinet.replayresult.PNRepResult;
import org.processmining.plugins.petrinet.replayresult.StepTypes;
import org.processmining.plugins.replayer.replayresult.SyncReplayResult;

/**
 * @author aadrians
 * 
 */

@Plugin(name = "Export result report as CSV (.csv)", returnLabels = {}, returnTypes = {}, parameterLabels = {
		"PNRepResult", "File" }, userAccessible = true)
@UIExportPlugin(description = "Export result report as CSV (.csv)", extension = "csv")
public class ExportPNRepResultChanged {
	// whole stats
	private int numReliableSynchronized = 0;
	private int numReliableModelOnlyInvi = 0;
	private int numReliableModelOnlyReal = 0;
	private int numReliableLogOnly = 0;
	private int numReliableViolations = 0;

	private int numCaseInvolved = 0;
	private int numReliableCaseInvolved;

	private NumberFormat nf = NumberFormat.getInstance();

	/**
	 * Pointers to property variable
	 */
	private static int RELIABLEMIN = 0;
	private static int RELIABLEMAX = 1;
	private static int MIN = 2;
	private static int MAX = 3;
	private static int SVAL = 4;
	private static int SVALRELIABLE = 5;
	private static int MVAL = 6;
	private static int MVALRELIABLE = 7;
	private static int PERFECTCASERELIABLECOUNTER = 8;

	@PluginVariant(variantLabel = "Export result report as CSV (.csv)", requiredParameterLabels = { 0, 1 })
	public void exportRepResult2File(PNRepResult res, File file, PetrinetGraph net, XLog log) throws IOException {
		exportToFile(res, file, net, log);
	}

	private void exportToFile(PNRepResult repResult, File file, PetrinetGraph net, XLog log) throws IOException {
		System.out.println("Exporting Result to file: ".concat(file.getPath()));
		System.gc();
		nf.setMinimumFractionDigits(2);
		nf.setMaximumFractionDigits(2);

		// export  to file
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file)));
		bw.write(createStringRep(repResult, net, log));
		bw.close();

	}

	private String createStringRep(PNRepResult repResult, PetrinetGraph net, XLog log) {
		String newLineChar = "\n";
		char commaSeparator = ',';
		XConceptExtension ce = XConceptExtension.instance();

		StringBuilder sb = new StringBuilder();
		sb.append("Result of replaying ");
		sb.append(net.getLabel());
		sb.append(" on ");
		sb.append(ce.extractName(log));
		sb.append(newLineChar);
		sb.append(newLineChar);

		sb.append("Index" + commaSeparator + "Case IDs" + commaSeparator + "NumOfCases" + commaSeparator + "IsReliable"
				+ commaSeparator);

		Iterator<SyncReplayResult> it = repResult.iterator();
		SyncReplayResult firstReplayResult = it.next();

		if (firstReplayResult != null) {
			// information for later
			Map<String, Double[]> calculations = new HashMap<String, Double[]>();

			// get all information exist in replay result
			Map<String, Double> infos = firstReplayResult.getInfo();
			Set<String> infoKeys = infos.keySet();
			String[] infoKeysArr = infoKeys.toArray(new String[infoKeys.size()]);
			for (String info : infoKeysArr) {
				sb.append("\"");
				sb.append(info);
				sb.append("\"");
				sb.append(commaSeparator);
			}

			sb.append("Match");
			sb.append(newLineChar);

			// now, create table content
			int indexCounter = 1;

			sb.append(indexCounter);
			sb.append(commaSeparator);

			String separatorStr = "";
			sb.append("\"");
			for (Integer index : firstReplayResult.getTraceIndex()) {
				sb.append(separatorStr);
				sb.append(ce.extractName(log.get(index)));
				separatorStr = "|";
			}
			sb.append("\"");
			
			sb.append(commaSeparator);

			sb.append(firstReplayResult.getTraceIndex().size());
			sb.append(commaSeparator);

			sb.append(firstReplayResult.isReliable() ? "Yes" : "No");
			sb.append(commaSeparator);

			for (String infoKey : infoKeysArr) {
				Double val = infos.get(infoKey);
				sb.append("\"");
				sb.append(nf.format(val));
				sb.append("\"");
				updateCalculations(firstReplayResult, infoKey, calculations, firstReplayResult.getTraceIndex().size(), val);
				sb.append(commaSeparator);
			}

			sb.append(generateMatchString(firstReplayResult));
			sb.append(newLineChar);

			indexCounter++;

			// add stats for first result
			numCaseInvolved += firstReplayResult.getTraceIndex().size();
			if (firstReplayResult.isReliable()) {
				numReliableCaseInvolved += numCaseInvolved;
			}

			while (it.hasNext()) {
				SyncReplayResult syncRepResult = it.next();
				int caseIDSize =syncRepResult.getTraceIndex().size();

				sb.append(indexCounter);
				sb.append(commaSeparator);

				separatorStr = "";
				sb.append("\"");
				for (Integer index : syncRepResult.getTraceIndex()) {
					sb.append(separatorStr);
					sb.append(ce.extractName(log.get(index)));
					separatorStr = "|";
				}
				sb.append("\"");
				sb.append(commaSeparator);

				sb.append(caseIDSize);
				sb.append(commaSeparator);

				sb.append(syncRepResult.isReliable() ? "Yes" : "No");
				sb.append(commaSeparator);

				Map<String, Double> allInfo = syncRepResult.getInfo();
				for (String infoKey : infoKeysArr) {
					Double val = allInfo.get(infoKey);
					sb.append("\"");
					sb.append(nf.format(val));
					sb.append("\"");
					updateCalculations(syncRepResult, infoKey, calculations, caseIDSize, val);
					sb.append(commaSeparator);
				}

				sb.append(generateMatchString(syncRepResult));
				sb.append(newLineChar);

				indexCounter++;

				// add stats
				numCaseInvolved += caseIDSize;
				if (syncRepResult.isReliable()) {
					numReliableCaseInvolved += caseIDSize;
				}

			}

			sb.append(newLineChar);
			sb.append(newLineChar);
			
			sb.append("ONLY RELIABLE RESULTS");
			
			sb.append(newLineChar);
			sb.append(newLineChar);

			// update based on calculations
			sb.append("Property");
			sb.append(commaSeparator);
			sb.append("Value");
			sb.append(newLineChar);
			
			sb.append("#Reliable cases replayed");
			sb.append(commaSeparator);
			sb.append(numReliableCaseInvolved);
			sb.append(newLineChar);
			
			sb.append("#Synchronous ev.class (log+model)");
			sb.append(commaSeparator);
			sb.append(numReliableSynchronized);
			sb.append(newLineChar);
			
			sb.append("#Skipped ev.class");
			sb.append(commaSeparator);
			sb.append(numReliableModelOnlyReal);
			sb.append(newLineChar);
			
			sb.append("#Unobservable ev.class");
			sb.append(commaSeparator);
			sb.append(numReliableModelOnlyInvi);
			sb.append(newLineChar);
			
			sb.append("#Inserted ev.class");
			sb.append(commaSeparator);
			sb.append(numReliableLogOnly);
			sb.append(newLineChar);
			
			Map<String, Object> info = repResult.getInfo();
			if (info != null){
				for (String key : info.keySet()){
					sb.append(key);
					sb.append(commaSeparator);
					sb.append(info.get(key));
					sb.append(newLineChar);
				}
			}
			
			sb.append(newLineChar);
			sb.append(newLineChar);

			
			// update based on calculations
			sb.append("Property");
			sb.append(commaSeparator);
			sb.append("Average");
			sb.append(commaSeparator);
			sb.append("Minimum");
			sb.append(commaSeparator);
			sb.append("Maximum");
			sb.append(commaSeparator);
			sb.append("Std. Deviation");
			sb.append(commaSeparator);
			sb.append("Number of cases with value 1.00");
			sb.append(newLineChar);

			// property based, only reliable results
			for (String property : calculations.keySet()) {
				Double[] values = calculations.get(property);
				sb.append("\"");
				sb.append(property);
				sb.append("\"");
				sb.append(commaSeparator);

				sb.append("\"");
				sb.append(numReliableCaseInvolved == 0 ? "<NaN>" : nf.format(values[MVALRELIABLE]));
				sb.append("\"");
				sb.append(commaSeparator);

				sb.append("\"");
				sb.append(numReliableCaseInvolved == 0 ? "<NaN>" : nf.format(values[RELIABLEMIN]));
				sb.append("\"");
				sb.append(commaSeparator);

				sb.append("\"");
				sb.append(numReliableCaseInvolved == 0 ? "<NaN>" : nf.format(values[RELIABLEMAX]));
				sb.append("\"");
				sb.append(commaSeparator);

				double reliabledev = numReliableCaseInvolved == 0 ? 0 : Math.sqrt(values[SVALRELIABLE] / (numReliableCaseInvolved - 1));
				sb.append("\"");
				sb.append(Double.compare(reliabledev, Double.NaN) == 0 ? "<NaN>" : nf.format(reliabledev));
				sb.append("\"");
				sb.append(commaSeparator);

				sb.append("\"");
				sb.append(nf.format(values[PERFECTCASERELIABLECOUNTER]));
				sb.append("\"");
				sb.append(newLineChar);
			}

		}
		return sb.toString();
	}

	private void updateCalculations(SyncReplayResult res, String property, Map<String, Double[]> calculations,
			int caseIDSize, double value) {
		// use it to calculate property
		Double[] oldValues = calculations.get(property);
		if (oldValues == null) {
			oldValues = new Double[] { Double.MAX_VALUE, Double.MIN_VALUE, Double.MAX_VALUE, Double.MIN_VALUE, 0.00000,
					0.00000, 0.00000, 0.00000, 0.00000 };
			calculations.put(property, oldValues);
		}

		if (Double.compare(oldValues[MIN], value) > 0) {
			oldValues[MIN] = value;
		}
		if (Double.compare(oldValues[MAX], value) < 0) {
			oldValues[MAX] = value;
		}

		int counterCaseIDSize = 0;
		if (numCaseInvolved == 0) {
			oldValues[MVAL] = value;
			oldValues[SVAL] = 0.0000;
			counterCaseIDSize++;
		}
		for (int i = counterCaseIDSize; i < caseIDSize; i++) {
			double oldMVal = oldValues[MVAL];
			// AA: previous average calculation is wrong. Fix this
			// oldValues[MVAL] += ((value - oldValues[MVAL]) / (i + numCaseInvolved));
			oldValues[MVAL] += ((value - oldValues[MVAL]) / (i + numCaseInvolved + 1));
			oldValues[SVAL] += ((value - oldMVal) * (value - oldValues[MVAL]));
		}

		if (res.isReliable()) {
			if (Double.compare(oldValues[RELIABLEMIN], value) > 0) {
				oldValues[RELIABLEMIN] = value;
			}
			if (Double.compare(oldValues[RELIABLEMAX], value) < 0) {
				oldValues[RELIABLEMAX] = value;
			}

			counterCaseIDSize = 0;
			if (numReliableCaseInvolved == 0) {
				oldValues[MVALRELIABLE] = value;
				oldValues[SVALRELIABLE] = 0.0000;
				counterCaseIDSize++;
			}
			for (int i = counterCaseIDSize; i < caseIDSize; i++) {
				double oldMVal = oldValues[MVALRELIABLE];
				// AA: previous average calculation is wrong. Fix this
				// oldValues[MVALRELIABLE] += ((value - oldValues[MVALRELIABLE]) / (i + numReliableCaseInvolved));
				oldValues[MVALRELIABLE] += ((value - oldValues[MVALRELIABLE]) / (i + numReliableCaseInvolved + 1));
				oldValues[SVALRELIABLE] += ((value - oldMVal) * (value - oldValues[MVALRELIABLE]));
			}

			if (Double.compare(value, 1.0000000) == 0) {
				oldValues[PERFECTCASERELIABLECOUNTER] += caseIDSize;
			}
		}
	}

	private Object generateMatchString(SyncReplayResult res) {
		StringBuilder sb = new StringBuilder();
		String limiter = "";
		List<Object> nodeInstance = res.getNodeInstance();

		int counter = 0;
		int size = res.getTraceIndex().size();
		for (StepTypes stepType : res.getStepTypes()) {
			sb.append(limiter);
			switch (stepType) {
				case L :
					sb.append("[L]");
					sb.append(nodeInstance.get(counter));
					if (res.isReliable()) {
						numReliableLogOnly += size;
					}
					;
					break;
				case MINVI :
					sb.append("[M-INVI]");
					sb.append(((Transition) nodeInstance.get(counter)).getLabel());
					if (res.isReliable()) {
						numReliableModelOnlyInvi += size;
					}
					;
					break;
				case MREAL :
					sb.append("[M-REAL]");
					sb.append(((Transition) nodeInstance.get(counter)).getLabel());
					if (res.isReliable()) {
						numReliableModelOnlyReal += size;
					}
					;
					break;
				case LMNOGOOD :
					sb.append("[L/M-NOGOOD]");
					sb.append(((Transition) nodeInstance.get(counter)).getLabel());
					if (res.isReliable()) {
						numReliableViolations += size;
					}
					;
					break;
				default :
					sb.append("[L/M]");
					sb.append(((Transition) nodeInstance.get(counter)).getLabel());
					if (res.isReliable()) {
						numReliableSynchronized += size;
					}
					;
			}
			limiter = "|";
			counter++;
		}
		return sb.toString();
	}
}
