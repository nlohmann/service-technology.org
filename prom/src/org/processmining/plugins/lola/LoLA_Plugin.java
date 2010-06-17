package org.processmining.plugins.lola;

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
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.configuration.Configuration;

import com.fluxicon.slickerbox.colors.SlickerColors;
import com.fluxicon.slickerbox.components.RoundedPanel;
import com.fluxicon.slickerbox.ui.SlickerRadioButtonUI;

/**
 * Plugin to execute LoLA.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @since March 2010
 * @version March 2010
 */
@Plugin(name = "LoLA: A Low-Level Petri Net Analyzer", //
		parameterLabels = { "a Petri net", "an initial marking" }, //
		returnLabels = "Analysis Results", //
		returnTypes = LoLADiagnosis.class, //
		userAccessible = true, //
		help = "Calls LoLA to analyze a Petri net.",
		mostSignificantResult = 1)
public class LoLA_Plugin {
	/// the chosen LoLA binary
	private LoLA_Executor lola = null;

	private JRadioButton boundednessButton;
	private JRadioButton deadlockButton;
	private JRadioButton homeButton;
	private JRadioButton reversibilityButton;
	
	// take net as input and guess initial marking
	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "K. Wolf and N. Lohmann", email = "lola@service-technology.org", website = "http://service-technology.org/lola")
	@PluginVariant(variantLabel = "LoLA: A Low-Level Petri Net Analyzer", requiredParameterLabels = { 1 })
	public LoLADiagnosis plug(UIPluginContext context, Petrinet net) throws Exception {
		Marking marking = context.tryToFindOrConstructFirstObject(Marking.class, InitialMarkingConnection.class, InitialMarkingConnection.MARKING, net);
		return work(context, net, marking);
	}

	// take net and initial marking as input
	@UITopiaVariant(affiliation = "Universit&#228;t Rostock", author = "K. Wolf and N. Lohmann", email = "lola@service-technology.org", website = "http://service-technology.org/lola")
	@PluginVariant(variantLabel = "LoLA: A Low-Level Petri Net Analyzer", requiredParameterLabels = { 0, 1 })
	public LoLADiagnosis plug(UIPluginContext context, Petrinet net, Marking marking) throws Exception {
		return work(context, net, marking);
	}
	
	// the actual working method
	private LoLADiagnosis work(UIPluginContext context, Petrinet net, Marking marking) throws Exception {
		// check if the net and marking are related
		context.getConnectionManager().getConnections(InitialMarkingConnection.class, context, net, marking);

		// ask user what we should do
		InteractionResult wish = getUserChoice(context);
		
		if (wish == InteractionResult.CANCEL) {
			throw new InterruptedException("Execution cancelled by user");
		} else {
			if (deadlockButton.isSelected()) {
				lola = LoLA_Executor.DEADLOCK;
				context.getFutureResult(0).setLabel("Deadlock Freedom Result for " + net.getLabel());
			}
			if (boundednessButton.isSelected()) {
				lola = LoLA_Executor.BOUNDEDNET;
				context.getFutureResult(0).setLabel("Boundedness Result for " + net.getLabel());
			}
			if (homeButton.isSelected()) {
				lola = LoLA_Executor.HOME;
				context.getFutureResult(0).setLabel("Home Marking Result for " + net.getLabel());
			}
			if (reversibilityButton.isSelected()) {
				context.getFutureResult(0).setLabel("Reversibility Result for " + net.getLabel());
				lola = LoLA_Executor.REVERSIBILITY;
			}
			if (lola == null) {
				return null;
			}
		}
		
		// call LoLA and get a results file
		Configuration cfg = lola.executeLoLA(context, net, marking);

		// visualize and return the results
		LoLADiagnosis vis = new LoLADiagnosis(context, net, cfg);
		return vis;
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
		
		boundednessButton = new JRadioButton("boundedness", true);
		deadlockButton    = new JRadioButton("deadlock freedom", false);
		homeButton = new JRadioButton("home markings", false);
		reversibilityButton = new JRadioButton("reversibility", false);
		boundednessButton.setUI(new SlickerRadioButtonUI());
		boundednessButton.setForeground(SlickerColors.COLOR_FG);
		boundednessButton.setOpaque(false);
		deadlockButton.setUI(new SlickerRadioButtonUI());
		deadlockButton.setForeground(SlickerColors.COLOR_FG);
		deadlockButton.setOpaque(false);
		homeButton.setUI(new SlickerRadioButtonUI());
		homeButton.setForeground(SlickerColors.COLOR_FG);
		homeButton.setOpaque(false);
		reversibilityButton.setUI(new SlickerRadioButtonUI());
		reversibilityButton.setForeground(SlickerColors.COLOR_FG);
		reversibilityButton.setOpaque(false);

		ButtonGroup bgroup = new ButtonGroup();
		bgroup.add(boundednessButton);
		bgroup.add(deadlockButton);
		bgroup.add(homeButton);
		bgroup.add(reversibilityButton);

		choicePanel.add(boundednessButton);
		choicePanel.add(deadlockButton);
		choicePanel.add(homeButton);
		choicePanel.add(reversibilityButton);
		choicePanel.add(Box.createHorizontalGlue());

		configPanel.add(headPanel, BorderLayout.CENTER);
		configPanel.add(choicePanel, BorderLayout.CENTER);

		return context.showConfiguration("LoLA: A Low-Level Petri Net Analyzer", configPanel);
	}
}
