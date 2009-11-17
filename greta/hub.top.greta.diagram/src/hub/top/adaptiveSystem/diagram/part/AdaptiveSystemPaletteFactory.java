package hub.top.adaptiveSystem.diagram.part;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.Tool;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gmf.runtime.diagram.ui.internal.services.palette.PaletteToolEntry;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeConnectionTool;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeCreationTool;

/**
 * @generated
 */
public class AdaptiveSystemPaletteFactory {

	/**
	 * @generated
	 */
	public void fillPalette(PaletteRoot paletteRoot) {
		paletteRoot.add(createNet1Group());
		paletteRoot.add(createElements2Group());
		paletteRoot.add(createConnection3Group());
	}

	/**
	 * Creates "Net" palette tool group
	 * @generated
	 */
	private PaletteContainer createNet1Group() {
		PaletteGroup paletteContainer = new PaletteGroup(
				hub.top.adaptiveSystem.diagram.part.Messages.Net1Group_title);
		paletteContainer.add(createAdaptiveProcess1CreationTool());
		paletteContainer.add(createOclet2CreationTool());
		return paletteContainer;
	}

	/**
	 * Creates "Elements" palette tool group
	 * @generated
	 */
	private PaletteContainer createElements2Group() {
		PaletteGroup paletteContainer = new PaletteGroup(
				hub.top.adaptiveSystem.diagram.part.Messages.Elements2Group_title);
		paletteContainer.add(createCondition1CreationTool());
		paletteContainer.add(createEvent2CreationTool());
		return paletteContainer;
	}

	/**
	 * Creates "Connection" palette tool group
	 * @generated
	 */
	private PaletteContainer createConnection3Group() {
		PaletteGroup paletteContainer = new PaletteGroup(
				hub.top.adaptiveSystem.diagram.part.Messages.Connection3Group_title);
		paletteContainer.add(createArc1CreationTool());
		return paletteContainer;
	}

	/**
	 * @generated
	 */
	private ToolEntry createAdaptiveProcess1CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(1);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.AdaptiveProcess_1001);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveProcess1CreationTool_title,
				hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveProcess1CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/AdaptiveProcess.gif")); //$NON-NLS-1$
		entry
				.setLargeIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/AdaptiveProcess.gif")); //$NON-NLS-1$
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createOclet2CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(1);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Oclet_1002);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.adaptiveSystem.diagram.part.Messages.Oclet2CreationTool_title,
				hub.top.adaptiveSystem.diagram.part.Messages.Oclet2CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Oclet.gif")); //$NON-NLS-1$
		entry
				.setLargeIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Oclet.gif")); //$NON-NLS-1$
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createCondition1CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(3);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.adaptiveSystem.diagram.part.Messages.Condition1CreationTool_title,
				hub.top.adaptiveSystem.diagram.part.Messages.Condition1CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Condition.gif")); //$NON-NLS-1$
		entry.setLargeIcon(entry.getSmallIcon());
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createEvent2CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(3);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.adaptiveSystem.diagram.part.Messages.Event2CreationTool_title,
				hub.top.adaptiveSystem.diagram.part.Messages.Event2CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Event.gif")); //$NON-NLS-1$
		entry
				.setLargeIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Event.gif")); //$NON-NLS-1$
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createArc1CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
		types
				.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
		LinkToolEntry entry = new LinkToolEntry(
				hub.top.adaptiveSystem.diagram.part.Messages.Arc1CreationTool_title,
				hub.top.adaptiveSystem.diagram.part.Messages.Arc1CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Arc.gif")); //$NON-NLS-1$
		entry
				.setLargeIcon(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.findImageDescriptor("/hub.top.GRETA/model/custom/icons/full/Arc.gif")); //$NON-NLS-1$
		return entry;
	}

	/**
	 * @generated NOT
	 */
	private static class NodeToolEntry extends PaletteToolEntry {
	  // original class inherited from ToolEntry
	  // PaletteToolEntry allows drag&drop from the tool palette

		/**
		 * @generated
		 */
		private final List elementTypes;

		/**
		 * @generated
		 */
		private NodeToolEntry(String title, String description,
				List elementTypes) {
			
		  // adapter super constructor call for PaletteToolEntry
		  //super(title, description, null, null);
		  super(null, title, null);          
		  this.setDescription(description);
		  
			this.elementTypes = elementTypes;
		}

		/**
		 * @generated
		 */
		public Tool createTool() {
			Tool tool = new UnspecifiedTypeCreationTool(elementTypes);
			tool.setProperties(getToolProperties());
			return tool;
		}
	}

	/**
	 * @generated
	 */
	private static class LinkToolEntry extends ToolEntry {

		/**
		 * @generated
		 */
		private final List relationshipTypes;

		/**
		 * @generated
		 */
		private LinkToolEntry(String title, String description,
				List relationshipTypes) {
			super(title, description, null, null);
			this.relationshipTypes = relationshipTypes;
		}

		/**
		 * @generated
		 */
		public Tool createTool() {
			Tool tool = new UnspecifiedTypeConnectionTool(relationshipTypes);
			tool.setProperties(getToolProperties());
			return tool;
		}
	}
}
