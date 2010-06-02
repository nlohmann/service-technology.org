package org.processmining.plugins.lola;

import java.util.SortedSet;
import java.util.TreeSet;

import org.processmining.framework.plugin.PluginContext;
import org.processmining.framework.plugin.annotations.Plugin;
import org.processmining.framework.plugin.annotations.PluginVariant;
import org.processmining.models.connections.petrinets.behavioral.BehavioralAnalysisInformationConnection;
import org.processmining.models.connections.petrinets.behavioral.MarkingNetConnection;
import org.processmining.models.connections.petrinets.behavioral.UnboundedPlacesConnection;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.analysis.NetAnalysisInformation;
import org.processmining.models.graphbased.directed.petrinet.analysis.UnboundedPlacesSet;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.semantics.petrinet.CTMarking;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.models.semantics.petrinet.impl.PetrinetSemanticsFactory;
import org.processmining.plugins.configuration.Configuration;
import org.processmining.plugins.configuration.ConfigurationList;

/**
 * Class to invoke LoLA to check whether a given net is bounded.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @version March 2010
 */
@Plugin(name = "Analyze Boundedness with LoLA", //
		parameterLabels = { "a Petri net", "an initial marking" }, //
		returnLabels = { "Boundedness info", "Unbounded Places", "Visualized Results" }, //
		returnTypes = { NetAnalysisInformation.class, UnboundedPlacesSet.class, LoLADiagnosis.class }, //
		userAccessible = true, //
		help = "Calls LoLA to check whether a given Petri net is bounded.",
		mostSignificantResult = 3)
public class LoLA_Boundedness {
	// choose the respective LoLA binary
	private LoLA_Executor lola = LoLA_Executor.BOUNDEDNET;

	// take net as input and guess initial marking
//	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "N. Lohmann", email = "niels.lohmann@uni-rostock.de", website = "http://service-technology.org/lola")
	@PluginVariant(variantLabel = "Analyze Boundedness with LoLA", requiredParameterLabels = { 0 })
	public Object[] plug(PluginContext context, Petrinet net) throws Exception {
		Marking marking = context.tryToFindOrConstructFirstObject(Marking.class, MarkingNetConnection.class, MarkingNetConnection.MARKING, net);
		return checkBoundedness(context, net, marking);
	}
	
	// take net and initial marking as input
	//	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "N. Lohmann", email = "niels.lohmann@uni-rostock.de", website = "http://service-technology.org/lola")
	@PluginVariant(variantLabel = "Analyze Boundedness with LoLA", requiredParameterLabels = { 0, 1 })
	public Object[] plug(PluginContext context, Petrinet net, Marking marking) throws Exception {
		return checkBoundedness(context, net, marking);
	}

	// do the actual work
	private Object[] checkBoundedness(PluginContext context, Petrinet net, Marking marking) throws Exception {
		// check if the net and marking are related
		context.getConnectionManager().getConnections(MarkingNetConnection.class, context, net, marking);

		// call LoLA and get a results file
		Configuration cfg = lola.executeLoLA(context, net, marking);

		// extract boundedness information from results file
		Boolean unbounded = cfg.getValue("unbounded.result", null);
		NetAnalysisInformation boundednessInfo = new NetAnalysisInformation("Boundedness Info");
		boundednessInfo.put(NetAnalysisInformation.BOUNDEDNESS, unbounded ? NetAnalysisInformation.FALSE : NetAnalysisInformation.TRUE);

		// extract unbounded places from results file
		UnboundedPlacesSet unboundedPlacesSet = new UnboundedPlacesSet();
		if (unbounded) {
			CTMarking omegaMarking = LoLA_Executor.constructMarking(net, (ConfigurationList) cfg.getValue("unbounded.state", null));
			SortedSet<Place> unboundedPlaces = new TreeSet<Place>();
			unboundedPlaces.addAll(omegaMarking.getOmegaPlaces());
			unboundedPlacesSet.add(unboundedPlaces);
		}

		LoLADiagnosis vis = new LoLADiagnosis(context, net, cfg);
		
		// connect results to context
		Object[] result = { boundednessInfo, unboundedPlacesSet, vis };
		context.addConnection(new BehavioralAnalysisInformationConnection(net, marking, PetrinetSemanticsFactory.regularPetrinetSemantics(Petrinet.class),
				(NetAnalysisInformation) result[0]));
		context.addConnection(new UnboundedPlacesConnection(net, (UnboundedPlacesSet) result[1], marking, PetrinetSemanticsFactory.regularPetrinetSemantics(Petrinet.class)));
		context.getFutureResult(0).setLabel("Boundedness Analysis of " + net.getLabel());
		context.getFutureResult(1).setLabel("Unbounded Places of " + net.getLabel());

		return result;
	}
}