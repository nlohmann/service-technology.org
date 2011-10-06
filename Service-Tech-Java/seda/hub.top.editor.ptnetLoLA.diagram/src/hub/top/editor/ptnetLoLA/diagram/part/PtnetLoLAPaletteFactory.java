/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.editor.ptnetLoLA.diagram.part;

import hub.top.editor.petrinets.diagram.tool.FireTool;
import hub.top.editor.ptnetLoLA.provider.PtnetLoLAEditPlugin;

import java.util.ArrayList;
import java.util.List;
import org.eclipse.gef.Tool;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeConnectionTool;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeCreationTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;

/**
 * @generated
 */
public class PtnetLoLAPaletteFactory {

	/**
	 * @generated
	 */
	public void fillPalette(PaletteRoot paletteRoot) {
		paletteRoot.add(createPtnetLoLA1Group());
		paletteRoot.add(createSimulationControl2Group());
	}

	/**
	 * Creates "ptnetLoLA" palette tool group
	 * @generated
	 */
	private PaletteContainer createPtnetLoLA1Group() {
		PaletteGroup paletteContainer = new PaletteGroup(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLA1Group_title);
		paletteContainer.add(createPlace1CreationTool());
		paletteContainer.add(createTransition2CreationTool());
		paletteContainer.add(createArc3CreationTool());
		return paletteContainer;
	}

	/**
	 * Creates "Simulation Control" palette tool group
	 * @generated
	 */
	private PaletteContainer createSimulationControl2Group() {
		PaletteGroup paletteContainer = new PaletteGroup(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.SimulationControl2Group_title);
		paletteContainer
				.setDescription(hub.top.editor.ptnetLoLA.diagram.part.Messages.SimulationControl2Group_desc);
		paletteContainer.add(createFireTransitionTool1CreationTool());
		return paletteContainer;
	}

	/**
	 * @generated
	 */
	private ToolEntry createPlace1CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(1);
		types
				.add(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Place_2002);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Place1CreationTool_title,
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Place1CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
						.getImageDescriptor(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Place_2002));
		entry.setLargeIcon(entry.getSmallIcon());
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createTransition2CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(1);
		types
				.add(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Transition_2001);
		NodeToolEntry entry = new NodeToolEntry(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Transition2CreationTool_title,
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Transition2CreationTool_desc,
				types);
		entry
				.setSmallIcon(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
						.getImageDescriptor(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Transition_2001));
		entry.setLargeIcon(entry.getSmallIcon());
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createArc3CreationTool() {
		List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
		types
				.add(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001);
		types
				.add(hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002);
		LinkToolEntry entry = new LinkToolEntry(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Arc3CreationTool_title,
				hub.top.editor.ptnetLoLA.diagram.part.Messages.Arc3CreationTool_desc,
				types);
		entry
				.setSmallIcon(AbstractUIPlugin.imageDescriptorFromPlugin(PtnetLoLAEditPlugin.ID, "icons/full/obj16/Arc.gif"));
		
		entry.setLargeIcon(entry.getSmallIcon());
		return entry;
	}

	/**
	 * @generated
	 */
	private ToolEntry createFireTransitionTool1CreationTool() {
		ToolEntry entry = new ToolEntry(
				hub.top.editor.ptnetLoLA.diagram.part.Messages.FireTransitionTool1CreationTool_title,
				hub.top.editor.ptnetLoLA.diagram.part.Messages.FireTransitionTool1CreationTool_desc,
				null, null) {
		};
		entry.setToolClass(FireTool.class);
		return entry;
	}

	/**
	 * @generated
	 */
	private static class NodeToolEntry extends ToolEntry {

		/**
		 * @generated
		 */
		private final List elementTypes;

		/**
		 * @generated
		 */
		private NodeToolEntry(String title, String description, List elementTypes) {
			super(title, description, null, null);
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
