/**
 * 
 */
package org.servicetech.prom;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

import org.deckfour.uitopia.api.event.TaskListener.InteractionResult;
import org.processmining.contexts.uitopia.UIPluginContext;
import org.processmining.contexts.uitopia.annotations.UITopiaVariant;
import org.processmining.framework.connections.ConnectionCannotBeObtained;
import org.processmining.framework.plugin.annotations.Plugin;
import org.processmining.framework.plugin.annotations.PluginVariant;
import org.processmining.models.connections.petrinets.behavioral.MarkingNetConnection;
import org.processmining.models.connections.petrinets.behavioral.UnfoldingNetConnection;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.models.graphbased.directed.petrinet.impl.PetrinetFactory;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.models.semantics.petrinet.PetrinetSemantics;
import org.processmining.models.semantics.petrinet.impl.PetrinetSemanticsFactory;
import hub.top.uma.DNode2Petrinet;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSys;
import hub.top.uma.DNodeSys_PtNet;
import hub.top.uma.InvalidModelException;

import com.fluxicon.slickerbox.colors.SlickerColors;
import com.fluxicon.slickerbox.components.RoundedPanel;
import com.fluxicon.slickerbox.ui.SlickerRadioButtonUI;

/**
 * Class to invoke Uma (an Unfolding-based Model Analyzer) to compute
 * a complete finite prefix of an unfolding of a given Petri net
 * 
 * @author Dirk Fahland
 * @email dirk.fahland@service-technology.org
 * @version Mar 23, 2010
 *
 */
@Plugin(name = "Uma2: an Unfolding-based Model Analyzer",
		parameterLabels = { "a Petri net", "an initial marking" }, //
		returnLabels = { "<html>complete prefix of the net's unfolding </html>" },
		returnTypes = Petrinet.class, 
		userAccessible = true,
		mostSignificantResult = 1)
public class UnfoldPN {
	
	private JRadioButton buildPrefixButton;
	private JRadioButton checkSoundnessFCButton;
	
	private JTextField	boundField;
	
	// take net as input and guess initial marking
	@UITopiaVariant(affiliation="Humboldt-Universit&#228;t zu Berlin", author="D. Fahland", email="dirk.fahland@service-technology.org", website = "http://service-technology.org/uma")
	@PluginVariant(variantLabel = "unfold Petri net to its complete finite prefix", requiredParameterLabels = { 0 })
	public Petrinet unfoldPetrinet(UIPluginContext context, Petrinet net) throws ConnectionCannotBeObtained, Exception {
		Marking initMarking = context.tryToFindOrConstructFirstObject(Marking.class, MarkingNetConnection.class, MarkingNetConnection.MARKING, net);
		return work(context, net, initMarking);
	}
	
	// take net and initial marking as input
	@UITopiaVariant(affiliation="Humboldt-Universit&#228;t zu Berlin", author="D. Fahland", email="dirk.fahland@service-technology.org", website = "http://service-technology.org/uma")
	@PluginVariant(variantLabel = "unfold Petri net to its complete finite prefix", requiredParameterLabels = { 0, 1 })
	public Petrinet unfoldPetrinet(UIPluginContext context, Petrinet net, Marking initMarking) throws ConnectionCannotBeObtained, Exception {
		return work(context, net, initMarking);
	}
		
	private Petrinet work(UIPluginContext context, Petrinet net, Marking initMarking) throws Exception {
		// check connection 
		context.getConnectionManager().getFirstConnection(MarkingNetConnection.class, context, net, initMarking);
		
		// ask user what we should do
		InteractionResult wish = getUserChoice(context);

		if (wish == InteractionResult.CANCEL) {
			return userCancel(context);
		} else {

			if (buildPrefixButton.isSelected()) {
				return buildPrefix(context, net, initMarking);
			} else if (checkSoundnessFCButton.isSelected()) {
				return checkSoundnessFC(context, net, initMarking);
			}
		}
		
		context.log("Uma did not do any work.");
		context.getFutureResult(0).cancel(true);
		return null;
	}

	/**
	 * build and return complete finite prefix of the given net
	 * 
	 * @param context
	 * @param net
	 * @param initMarking
	 * @return
	 * @throws Exception
	 */
	private Petrinet buildPrefix(UIPluginContext context, Petrinet net, Marking initMarking) throws Exception {
		
		// load and initialize input Petri net
		PetrinetSemantics semantics = PetrinetSemanticsFactory.regularPetrinetSemantics(Petrinet.class);
		semantics.initialize(net.getTransitions(), initMarking);

		try {
			DNodeSys sys = new DNodeSys_PtNet(net, initMarking);
			
			DNodeBP buildPrefix = new DNodeBP(sys);
			buildPrefix.configure_buildOnly();
			buildPrefix.configure_PetriNet();
			if (boundField.getText() != null && boundField.getText().length() > 0) {
				try {
					int bound = Integer.parseInt(boundField.getText());
					if (bound >= 0) buildPrefix.configure_setBound(bound);
				} catch (NumberFormatException e) {
					// just ignore invalid settings
				}
			}
			
			boolean interrupted = false;
			while (buildPrefix.step() > 0) {
				if (context.getProgress().isCancelled()) {
					interrupted = true;
					break;
				}
			}
	
			// Step 2. construct net unfolding from the coverability graph
			//Object [] result = constructPrefix(net, initMarking, covGraph, startStateSet);
			Petrinet result = DNode2Petrinet.process(buildPrefix);
	
			if (!interrupted) {
				// set label before result output
				context.getFutureResult(0).setLabel("Complete prefix of the unfolding of " + net.getLabel());
			} else {
				// set label before result output
				context.getFutureResult(0).setLabel("Incomplete prefix of the unfolding of " + net.getLabel()+" (cancelled by user)");
			}
			
			// connect the result
			context.addConnection(new UnfoldingNetConnection(net, initMarking, semantics, result));
		
			return result;
		} catch (InvalidModelException e) {
			// the model is invalid for the unfolding algorithm
			context.log(e.getMessage());
			context.getFutureResult(0).cancel(true);
			return null ;
		}
	}
	
	/**
	 * build prefix and check for soundness of free-choice net
	 * 
	 * @param context
	 * @param net
	 * @param initMarking
	 * @return
	 * @throws Exception
	 */
	private Petrinet checkSoundnessFC(UIPluginContext context, Petrinet net, Marking initMarking) throws Exception {
		
		// load and initialize input Petri net
		PetrinetSemantics semantics = PetrinetSemanticsFactory.regularPetrinetSemantics(Petrinet.class);
		semantics.initialize(net.getTransitions(), initMarking);

		try {
			DNodeSys sys = new DNodeSys_PtNet(net, initMarking);
			
			DNodeBP buildPrefix = new DNodeBP(sys);
			buildPrefix.configure_checkSoundness();
			buildPrefix.configure_PetriNet();
			
			boolean interrupted = false;
			while (buildPrefix.step() > 0) {
				if (context.getProgress().isCancelled()) {
					interrupted = true;
					break;
				}
			}
			
			Petrinet result;
			
			if (!interrupted) {
	            boolean hasDeadlock = buildPrefix.hasDeadCondition();
	            boolean isUnsafe = !buildPrefix.isSafe();
	            
	            if (hasDeadlock || isUnsafe) {
	            	// return constructed prefix of unsound net
					result = DNode2Petrinet.process(buildPrefix);
					
					// set label before result output
					context.getFutureResult(0).setLabel("Counterexample to unsoundness of " + net.getLabel());
	            } else {
	            	Petrinet netResult = PetrinetFactory.newPetrinet("net is sound");
	            	
	            	Place alpha = netResult.addPlace("alpha");
	            	Place omega = netResult.addPlace("omega");
	            	Transition sound = netResult.addTransition("sound");
	            	netResult.addArc(alpha, sound);
	            	netResult.addArc(sound, omega);
	            	result = netResult;
	            	
					// set label before result output
					context.getFutureResult(0).setLabel("Sound: " + net.getLabel());
	            }
			} else {
            	// return constructed prefix of unsound net
				result = DNode2Petrinet.process(buildPrefix);
				
				// set label before result output
				context.getFutureResult(0).setLabel("Incomplete prefix of the unfolding of " + net.getLabel()+" (cancelled by user)");
			}
			
			// connect the result
			context.addConnection(new UnfoldingNetConnection(net, initMarking, semantics, result));
		
			return result;
		} catch (InvalidModelException e) {
			// the model is invalid for the unfolding algorithm
			context.log(e.getMessage());
			context.getFutureResult(0).cancel(true);
			return null ;
		}
	}
	
	private static Color COLOR_WHITE = new Color(255,255,255);

	
	// display a dialog to ask use what to do
	private InteractionResult getUserChoice(UIPluginContext context) {
		JPanel configPanel = new JPanel();
		configPanel.removeAll();
		configPanel.setLayout(new BoxLayout(configPanel, BoxLayout.Y_AXIS));
		configPanel.setBorder(BorderFactory.createEmptyBorder());
		
		configPanel.setMaximumSize(new Dimension(800, 500));
		configPanel.setMinimumSize(new Dimension(230, 180));
		configPanel.setPreferredSize(new Dimension(250, 180));
		
		JPanel headPanel = new RoundedPanel();
		headPanel.setLayout(new BoxLayout(headPanel, BoxLayout.X_AXIS));
		headPanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		headPanel.setMinimumSize(new Dimension(350, 50));
		headPanel.setMaximumSize(new Dimension(1000, 50));
		headPanel.setPreferredSize(new Dimension(400, 50));
		headPanel.setBackground(SlickerColors.COLOR_BG_1);
		
			JLabel headLabel = new JLabel("Please choose the verification task");
			headLabel.setFont(new Font("SansSerif", Font.BOLD, 16));
			
		headPanel.add(Box.createHorizontalGlue());
		headPanel.add(headLabel);
		headPanel.add(Box.createHorizontalGlue());
		
		JPanel buildPrefixPanel = new RoundedPanel();
		buildPrefixPanel.setLayout(new BoxLayout(buildPrefixPanel, BoxLayout.X_AXIS));
		buildPrefixPanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		buildPrefixPanel.setMinimumSize(new Dimension(350, 50));
		buildPrefixPanel.setMaximumSize(new Dimension(1000, 50));
		buildPrefixPanel.setPreferredSize(new Dimension(400, 50));
		buildPrefixPanel.setBackground(SlickerColors.COLOR_BG_2);
		
		
			buildPrefixButton = new JRadioButton("construct prefix", true);
			buildPrefixButton.setUI(new SlickerRadioButtonUI());
			buildPrefixButton.setOpaque(false);
			buildPrefixButton.setForeground(SlickerColors.COLOR_FG);
			//buildPrefixButton.setMaximumSize(new Dimension(200,20));
			//buildPrefixButton.setPreferredSize(new Dimension(200,20));
			
			JLabel boundLabel = new JLabel("bound k = ");
			//filterLabel.setForeground(new Color(160, 160, 160));
			boundLabel.setForeground(SlickerColors.COLOR_FG);
			boundLabel.setOpaque(false);
			boundField = new JTextField("1");
			boundField.setOpaque(true);
			boundField.setBackground(COLOR_WHITE);
			boundField.setMinimumSize(new Dimension(40,20));
			boundField.setMaximumSize(new Dimension(40,20));
			boundField.setPreferredSize(new Dimension(40,20));
			
		buildPrefixPanel.add(buildPrefixButton);
		buildPrefixPanel.add(Box.createHorizontalGlue());
		buildPrefixPanel.add(boundLabel);
		buildPrefixPanel.add(boundField);
		
		JPanel checkSoundnessPanel = new RoundedPanel();
		checkSoundnessPanel.setLayout(new BoxLayout(checkSoundnessPanel, BoxLayout.X_AXIS));
		checkSoundnessPanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15));
		checkSoundnessPanel.setMinimumSize(new Dimension(350, 50));
		checkSoundnessPanel.setMaximumSize(new Dimension(1000, 50));
		checkSoundnessPanel.setPreferredSize(new Dimension(400, 50));
		checkSoundnessPanel.setBackground(SlickerColors.COLOR_BG_2);

			checkSoundnessFCButton = new JRadioButton("check soundness (free-choice net)", false);
			checkSoundnessFCButton.setUI(new SlickerRadioButtonUI());
			checkSoundnessFCButton.setOpaque(false);
			checkSoundnessFCButton.setForeground(SlickerColors.COLOR_FG);

		checkSoundnessPanel.add(checkSoundnessFCButton);
		checkSoundnessPanel.add(Box.createHorizontalGlue());


		ButtonGroup bgroup = new ButtonGroup();
		bgroup.add(buildPrefixButton);
		bgroup.add(checkSoundnessFCButton);

		configPanel.add(headPanel, BorderLayout.CENTER);
		configPanel.add(buildPrefixPanel, BorderLayout.CENTER);
		configPanel.add(checkSoundnessPanel, BorderLayout.CENTER);

		return context.showConfiguration("Uma: an Unfolding-based Model Analyzer", configPanel);
	}
	
	/**
	 * Generate proper cancelling information for Uma. 
	 * @param context
	 * @return
	 */
	private Petrinet userCancel(UIPluginContext context) {
		context.log("The user has cancelled Uma.");
		context.getFutureResult(0).cancel(true);
		return null;
	}
}