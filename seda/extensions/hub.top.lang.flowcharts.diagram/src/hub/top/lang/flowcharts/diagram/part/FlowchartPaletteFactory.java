/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 *  		Dirk Fahland
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
 */
package hub.top.lang.flowcharts.diagram.part;

import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.Tool;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeConnectionTool;
import org.eclipse.gmf.runtime.diagram.ui.tools.UnspecifiedTypeCreationTool;

/**
 * @generated
 */
public class FlowchartPaletteFactory {

  /**
   * @generated
   */
  public void fillPalette(PaletteRoot paletteRoot) {
    paletteRoot.add(createFlow1Group());
  }

  /**
   * Creates "flow" palette tool group
   * @generated
   */
  private PaletteContainer createFlow1Group() {
    PaletteGroup paletteContainer = new PaletteGroup(Messages.Flow1Group_title);
    paletteContainer.add(createStartNode1CreationTool());
    paletteContainer.add(createEndnode2CreationTool());
    paletteContainer.add(createSplitNode3CreationTool());
    paletteContainer.add(createMergeNode4CreationTool());
    paletteContainer.add(createFlow5CreationTool());
    paletteContainer.add(createSimpleActivity6CreationTool());
    paletteContainer.add(createPersistentResource7CreationTool());
    paletteContainer.add(createTransientResource8CreationTool());
    paletteContainer.add(createDocument9CreationTool());
    paletteContainer.add(createEvent10CreationTool());
    paletteContainer.add(createComplexActivity11CreationTool());
    paletteContainer.add(createLink12CreationTool());
    return paletteContainer;
  }

  /**
   * @generated
   */
  private ToolEntry createStartNode1CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.StartNode_2001);
    types.add(FlowchartElementTypes.StartNode_3001);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.StartNode1CreationTool_title,
        Messages.StartNode1CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.StartNode_2001));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createEndnode2CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.Endnode_2002);
    types.add(FlowchartElementTypes.Endnode_3002);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.Endnode2CreationTool_title,
        Messages.Endnode2CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.Endnode_2002));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createSplitNode3CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.SplitNode_2006);
    types.add(FlowchartElementTypes.SplitNode_3006);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.SplitNode3CreationTool_title,
        Messages.SplitNode3CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.SplitNode_2006));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createMergeNode4CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.MergeNode_2007);
    types.add(FlowchartElementTypes.MergeNode_3007);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.MergeNode4CreationTool_title,
        Messages.MergeNode4CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.MergeNode_2007));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createFlow5CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.Flow_2008);
    types.add(FlowchartElementTypes.Flow_3008);
    NodeToolEntry entry = new NodeToolEntry(Messages.Flow5CreationTool_title,
        Messages.Flow5CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.Flow_2008));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createSimpleActivity6CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.SimpleActivity_2003);
    types.add(FlowchartElementTypes.SimpleActivity_3003);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.SimpleActivity6CreationTool_title,
        Messages.SimpleActivity6CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.SimpleActivity_2003));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createPersistentResource7CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.PersistentResource_3011);
    types.add(FlowchartElementTypes.PersistentResource_2011);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.PersistentResource7CreationTool_title,
        Messages.PersistentResource7CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.PersistentResource_3011));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createTransientResource8CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.TransientResource_3009);
    types.add(FlowchartElementTypes.TransientResource_2009);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.TransientResource8CreationTool_title,
        Messages.TransientResource8CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.TransientResource_3009));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createDocument9CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.Document_3010);
    types.add(FlowchartElementTypes.Document_2010);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.Document9CreationTool_title,
        Messages.Document9CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.Document_3010));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createEvent10CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.Event_2005);
    types.add(FlowchartElementTypes.Event_3005);
    NodeToolEntry entry = new NodeToolEntry(Messages.Event10CreationTool_title,
        Messages.Event10CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.Event_2005));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createComplexActivity11CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(2);
    types.add(FlowchartElementTypes.Subprocess_2004);
    types.add(FlowchartElementTypes.Subprocess_3004);
    NodeToolEntry entry = new NodeToolEntry(
        Messages.ComplexActivity11CreationTool_title,
        Messages.ComplexActivity11CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.Subprocess_2004));
    entry.setLargeIcon(entry.getSmallIcon());
    return entry;
  }

  /**
   * @generated
   */
  private ToolEntry createLink12CreationTool() {
    List/*<IElementType>*/types = new ArrayList/*<IElementType>*/(1);
    types.add(FlowchartElementTypes.DiagramArc_4001);
    LinkToolEntry entry = new LinkToolEntry(Messages.Link12CreationTool_title,
        Messages.Link12CreationTool_desc, types);
    entry.setSmallIcon(FlowchartElementTypes
        .getImageDescriptor(FlowchartElementTypes.DiagramArc_4001));
    entry.setLargeIcon(entry.getSmallIcon());
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
