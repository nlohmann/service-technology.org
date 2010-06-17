package org.processmining.plugins.wendy;

import java.awt.BorderLayout;
import java.awt.Font;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import org.deckfour.uitopia.api.event.TaskListener.InteractionResult;
import org.processmining.contexts.uitopia.UIPluginContext;
import org.processmining.contexts.uitopia.annotations.UITopiaVariant;
import org.processmining.framework.plugin.annotations.Plugin;
import org.processmining.framework.plugin.annotations.PluginVariant;
import org.processmining.models.connections.petrinets.behavioral.InitialMarkingConnection;
import org.processmining.models.connections.transitionsystem.TransitionSystemConnection;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.transitionsystem.AcceptStateSet;
import org.processmining.models.graphbased.directed.transitionsystem.StartStateSet;
import org.processmining.models.graphbased.directed.transitionsystem.TransitionSystem;
import org.processmining.models.graphbased.directed.transitionsystem.TransitionSystemFactory;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.configuration.Configuration;
import org.processmining.plugins.configuration.ConfigurationList;

import com.fluxicon.slickerbox.colors.SlickerColors;
import com.fluxicon.slickerbox.components.RoundedPanel;
import com.fluxicon.slickerbox.ui.SlickerRadioButtonUI;

/**
 * Plugin to execute Wendy.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @since March 2010
 * @version March 2010
 */
@Plugin(name = "Wendy: Synthesizing Partners for Services", //
		parameterLabels = { "an open net", "an initial marking" }, //
		returnLabels = { "synthesis result", "initial state", "final states" }, //
		returnTypes = { TransitionSystem.class, StartStateSet.class, AcceptStateSet.class }, //
		userAccessible = true, //
		help = "Calls Wendy to analyze an open net.",
		mostSignificantResult = 1)
public class Wendy_Plugin {

	/// whether you chose "operating guidelines"
	private JRadioButton ogButton;
	/// whether you chose "most-permissive partner"
	private JRadioButton mpButton;
	/// whether you chose "reduced partner" (a.k.a. "interaction graph")
	private JRadioButton igButton;
	
	private int mode;
	
	// take net as input and guess initial marking
	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "N. Lohmann and D. Weinberg", email = "wendy@service-technology.org", website = "http://service-technology.org/wendy")
	@PluginVariant(variantLabel = "Wendy: Synthesizing Partners for Services", requiredParameterLabels = { 1 })
	public Object[] plug(UIPluginContext context, Petrinet net) throws Exception {
		Marking marking = context.tryToFindOrConstructFirstObject(Marking.class, InitialMarkingConnection.class, InitialMarkingConnection.MARKING, net);
		return work(context, net, marking);
	}

	// take net and initial marking as input
	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "N. Lohmann and K. Wolf", email = "wendy@service-technology.org", website = "http://service-technology.org/wendy")
	@PluginVariant(variantLabel = "Wendy: Synthesizing Partners for Services", requiredParameterLabels = { 0, 1 })
	public Object[] plug(UIPluginContext context, Petrinet net, Marking marking) throws Exception {
		return work(context, net, marking);
	}
	
	// the actual working method
	private Object[] work(UIPluginContext context, Petrinet net, Marking marking) throws Exception {
		// check if the net and marking are related
		context.getConnectionManager().getConnections(InitialMarkingConnection.class, context, net, marking);

		// ask user what we should do
		InteractionResult wish = getUserChoice(context);

		String tsname = "";
		
		if (wish == InteractionResult.CANCEL) {
			throw new InterruptedException("Execution cancelled by user");
		} else {
			if (ogButton.isSelected()) {
				mode = 0;
				tsname = "Operating Guidelines";
			}
			if (mpButton.isSelected()) {
				mode = 1;
				tsname = "Most-permissive Partner";
			}
			if (igButton.isSelected()) {
				mode = 2;
				tsname = "Reduced Partner";
			}
		}

		TransitionSystem ts = TransitionSystemFactory.newTransitionSystem(tsname);;

		// call Wendy and get a results file
		Wendy_Executor wendy = new Wendy_Executor(mode);
		Configuration cfg = wendy.executeWendy(context, net, marking);

		ConfigurationList states = null;
		ConfigurationList initialStates = null;
		ConfigurationList finalStates = null;
		
		// connect results output
		if (mode == 0) {
			states = cfg.getValue("og.states", null);
			initialStates = cfg.getValue("og.initial_states", null);
		} else {
			states = cfg.getValue("sa.states", null);
			initialStates = cfg.getValue("sa.initial_states", null);
			finalStates = cfg.getValue("sa.final_states", null);
		}
		
		// copy states and transitions
		for (int state = 0; state < states.size(); state++) {
			Integer stateId = states.getValue(state + ".id", null);
			ts.addState(stateId);
			ConfigurationList successors = states.getValue(state + ".successors", null);
			for (int successor = 0; successor < successors.size(); successor++) {
				String label = successors.getValue(successor + ".0", null);
				Integer succId = successors.getValue(successor + ".1", null);
				ts.addState(succId);
				ts.addTransition(stateId, succId, label);
			}
		}

		StartStateSet starts = new StartStateSet();
		for (int state = 0; state < initialStates.size(); state++) {
			Integer stateId = initialStates.getValue(state + ".0", null);
			starts.add(stateId);
		}
		
		AcceptStateSet accept = new AcceptStateSet();
		if (finalStates != null) {
			for (int state = 0; state < finalStates.size(); state++) {
				Integer stateId = finalStates.getValue(state + ".0", null);
				accept.add(stateId);
			}
		}
		
		context.getConnectionManager().addConnection(new TransitionSystemConnection(ts, starts, accept));
		context.getFutureResult(0).setLabel(tsname + " of " + net.getLabel());
		context.getFutureResult(1).setLabel("Initial state of " + context.getFutureResult(0).getLabel());
		context.getFutureResult(2).setLabel("Final states of " + context.getFutureResult(0).getLabel());
		return new Object[] { ts, starts, accept };
	}

	/// display a dialog to ask user what to do
	private InteractionResult getUserChoice(UIPluginContext context) {
		JPanel configPanel = new JPanel();
		configPanel.removeAll();
		configPanel.setLayout(new BoxLayout(configPanel, BoxLayout.Y_AXIS));
		configPanel.setBorder(BorderFactory.createEmptyBorder());
		
		JPanel headPanel = new RoundedPanel();
		headPanel.setLayout(new BoxLayout(headPanel, BoxLayout.Y_AXIS));
		headPanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		headPanel.setBackground(SlickerColors.COLOR_BG_1);
		
		JLabel headLabel = new JLabel("Please choose the verification task");
		headLabel.setFont(new Font("SansSerif", Font.BOLD, 16));
		headPanel.add(Box.createHorizontalGlue());
		headPanel.add(headLabel);
		headPanel.add(Box.createHorizontalGlue());
		
		JPanel choicePanel = new RoundedPanel();
		choicePanel.setLayout(new BoxLayout(choicePanel, BoxLayout.Y_AXIS));
		choicePanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		choicePanel.setBackground(SlickerColors.COLOR_BG_2);
		
		ogButton = new JRadioButton("operating guidelines", true);
		mpButton = new JRadioButton("most-permissive partner", false);
		igButton = new JRadioButton("reduced partner", false);
		ogButton.setUI(new SlickerRadioButtonUI());
		ogButton.setForeground(SlickerColors.COLOR_FG);
		ogButton.setOpaque(false);
		mpButton.setUI(new SlickerRadioButtonUI());
		mpButton.setForeground(SlickerColors.COLOR_FG);
		mpButton.setOpaque(false);
		igButton.setUI(new SlickerRadioButtonUI());
		igButton.setForeground(SlickerColors.COLOR_FG);
		igButton.setOpaque(false);

		ButtonGroup bgroup = new ButtonGroup();
		bgroup.add(ogButton);
		bgroup.add(mpButton);
		bgroup.add(igButton);

		choicePanel.add(ogButton);
		choicePanel.add(mpButton);
		choicePanel.add(igButton);
		choicePanel.add(Box.createHorizontalGlue());

		configPanel.add(headPanel, BorderLayout.CENTER);
		configPanel.add(choicePanel, BorderLayout.CENTER);
		
		return context.showConfiguration("Wendy: Synthesizing Partners for Services", configPanel);
	}
}
