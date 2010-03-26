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
package hub.top.lang.flowcharts.diagram.navigator;

import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Document2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Endnode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Event2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EventEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Flow2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.MergeNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.MergeNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivity2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Subprocess2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;
import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;
import hub.top.lang.flowcharts.diagram.providers.FlowchartParserProvider;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserOptions;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserService;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.viewers.ITreePathLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.ViewerLabel;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.navigator.ICommonContentExtensionSite;
import org.eclipse.ui.navigator.ICommonLabelProvider;

/**
 * @generated
 */
public class FlowchartNavigatorLabelProvider extends LabelProvider implements
    ICommonLabelProvider, ITreePathLabelProvider {

  /**
   * @generated
   */
  static {
    FlowchartDiagramEditorPlugin
        .getInstance()
        .getImageRegistry()
        .put(
            "Navigator?UnknownElement", ImageDescriptor.getMissingImageDescriptor()); //$NON-NLS-1$
    FlowchartDiagramEditorPlugin.getInstance().getImageRegistry().put(
        "Navigator?ImageNotFound", ImageDescriptor.getMissingImageDescriptor()); //$NON-NLS-1$
  }

  /**
   * @generated
   */
  public void updateLabel(ViewerLabel label, TreePath elementPath) {
    Object element = elementPath.getLastSegment();
    if (element instanceof FlowchartNavigatorItem
        && !isOwnView(((FlowchartNavigatorItem) element).getView())) {
      return;
    }
    label.setText(getText(element));
    label.setImage(getImage(element));
  }

  /**
   * @generated
   */
  public Image getImage(Object element) {
    if (element instanceof FlowchartNavigatorGroup) {
      FlowchartNavigatorGroup group = (FlowchartNavigatorGroup) element;
      return FlowchartDiagramEditorPlugin.getInstance().getBundledImage(
          group.getIcon());
    }

    if (element instanceof FlowchartNavigatorItem) {
      FlowchartNavigatorItem navigatorItem = (FlowchartNavigatorItem) element;
      if (!isOwnView(navigatorItem.getView())) {
        return super.getImage(element);
      }
      return getImage(navigatorItem.getView());
    }

    return super.getImage(element);
  }

  /**
   * @generated
   */
  public Image getImage(View view) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case WorkflowDiagramEditPart.VISUAL_ID:
      return getImage(
          "Navigator?Diagram?http://hu-berlin.de/flowchart?WorkflowDiagram", FlowchartElementTypes.WorkflowDiagram_79); //$NON-NLS-1$
    case StartNodeEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?StartNode", FlowchartElementTypes.StartNode_1001); //$NON-NLS-1$
    case EndnodeEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?Endnode", FlowchartElementTypes.Endnode_1002); //$NON-NLS-1$
    case SimpleActivityEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?SimpleActivity", FlowchartElementTypes.SimpleActivity_1003); //$NON-NLS-1$
    case SubprocessEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?Subprocess", FlowchartElementTypes.Subprocess_1004); //$NON-NLS-1$
    case EventEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?Event", FlowchartElementTypes.Event_1005); //$NON-NLS-1$
    case SplitNodeEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?SplitNode", FlowchartElementTypes.SplitNode_1006); //$NON-NLS-1$
    case MergeNodeEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?MergeNode", FlowchartElementTypes.MergeNode_1007); //$NON-NLS-1$
    case FlowEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?Flow", FlowchartElementTypes.Flow_1008); //$NON-NLS-1$
    case TransientResourceEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?TransientResource", FlowchartElementTypes.TransientResource_1009); //$NON-NLS-1$
    case DocumentEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?Document", FlowchartElementTypes.Document_1010); //$NON-NLS-1$
    case PersistentResourceEditPart.VISUAL_ID:
      return getImage(
          "Navigator?TopLevelNode?http://hu-berlin.de/flowchart?PersistentResource", FlowchartElementTypes.PersistentResource_1011); //$NON-NLS-1$
    case StartNode2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?StartNode", FlowchartElementTypes.StartNode_2001); //$NON-NLS-1$
    case Endnode2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?Endnode", FlowchartElementTypes.Endnode_2002); //$NON-NLS-1$
    case SimpleActivity2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?SimpleActivity", FlowchartElementTypes.SimpleActivity_2003); //$NON-NLS-1$
    case Subprocess2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?Subprocess", FlowchartElementTypes.Subprocess_2004); //$NON-NLS-1$
    case Event2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?Event", FlowchartElementTypes.Event_2005); //$NON-NLS-1$
    case SplitNode2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?SplitNode", FlowchartElementTypes.SplitNode_2006); //$NON-NLS-1$
    case MergeNode2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?MergeNode", FlowchartElementTypes.MergeNode_2007); //$NON-NLS-1$
    case Flow2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?Flow", FlowchartElementTypes.Flow_2008); //$NON-NLS-1$
    case TransientResource2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?TransientResource", FlowchartElementTypes.TransientResource_2009); //$NON-NLS-1$
    case Document2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?Document", FlowchartElementTypes.Document_2010); //$NON-NLS-1$
    case PersistentResource2EditPart.VISUAL_ID:
      return getImage(
          "Navigator?Node?http://hu-berlin.de/flowchart?PersistentResource", FlowchartElementTypes.PersistentResource_2011); //$NON-NLS-1$
    case DiagramArcEditPart.VISUAL_ID:
      return getImage(
          "Navigator?Link?http://hu-berlin.de/flowchart?DiagramArc", FlowchartElementTypes.DiagramArc_3001); //$NON-NLS-1$
    }
    return getImage("Navigator?UnknownElement", null); //$NON-NLS-1$
  }

  /**
   * @generated
   */
  private Image getImage(String key, IElementType elementType) {
    ImageRegistry imageRegistry = FlowchartDiagramEditorPlugin.getInstance()
        .getImageRegistry();
    Image image = imageRegistry.get(key);
    if (image == null && elementType != null
        && FlowchartElementTypes.isKnownElementType(elementType)) {
      image = FlowchartElementTypes.getImage(elementType);
      imageRegistry.put(key, image);
    }

    if (image == null) {
      image = imageRegistry.get("Navigator?ImageNotFound"); //$NON-NLS-1$
      imageRegistry.put(key, image);
    }
    return image;
  }

  /**
   * @generated
   */
  public String getText(Object element) {
    if (element instanceof FlowchartNavigatorGroup) {
      FlowchartNavigatorGroup group = (FlowchartNavigatorGroup) element;
      return group.getGroupName();
    }

    if (element instanceof FlowchartNavigatorItem) {
      FlowchartNavigatorItem navigatorItem = (FlowchartNavigatorItem) element;
      if (!isOwnView(navigatorItem.getView())) {
        return null;
      }
      return getText(navigatorItem.getView());
    }

    return super.getText(element);
  }

  /**
   * @generated
   */
  public String getText(View view) {
    if (view.getElement() != null && view.getElement().eIsProxy()) {
      return getUnresolvedDomainElementProxyText(view);
    }
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case WorkflowDiagramEditPart.VISUAL_ID:
      return getWorkflowDiagram_79Text(view);
    case StartNodeEditPart.VISUAL_ID:
      return getStartNode_1001Text(view);
    case EndnodeEditPart.VISUAL_ID:
      return getEndnode_1002Text(view);
    case SimpleActivityEditPart.VISUAL_ID:
      return getSimpleActivity_1003Text(view);
    case SubprocessEditPart.VISUAL_ID:
      return getSubprocess_1004Text(view);
    case EventEditPart.VISUAL_ID:
      return getEvent_1005Text(view);
    case SplitNodeEditPart.VISUAL_ID:
      return getSplitNode_1006Text(view);
    case MergeNodeEditPart.VISUAL_ID:
      return getMergeNode_1007Text(view);
    case FlowEditPart.VISUAL_ID:
      return getFlow_1008Text(view);
    case TransientResourceEditPart.VISUAL_ID:
      return getTransientResource_1009Text(view);
    case DocumentEditPart.VISUAL_ID:
      return getDocument_1010Text(view);
    case PersistentResourceEditPart.VISUAL_ID:
      return getPersistentResource_1011Text(view);
    case StartNode2EditPart.VISUAL_ID:
      return getStartNode_2001Text(view);
    case Endnode2EditPart.VISUAL_ID:
      return getEndnode_2002Text(view);
    case SimpleActivity2EditPart.VISUAL_ID:
      return getSimpleActivity_2003Text(view);
    case Subprocess2EditPart.VISUAL_ID:
      return getSubprocess_2004Text(view);
    case Event2EditPart.VISUAL_ID:
      return getEvent_2005Text(view);
    case SplitNode2EditPart.VISUAL_ID:
      return getSplitNode_2006Text(view);
    case MergeNode2EditPart.VISUAL_ID:
      return getMergeNode_2007Text(view);
    case Flow2EditPart.VISUAL_ID:
      return getFlow_2008Text(view);
    case TransientResource2EditPart.VISUAL_ID:
      return getTransientResource_2009Text(view);
    case Document2EditPart.VISUAL_ID:
      return getDocument_2010Text(view);
    case PersistentResource2EditPart.VISUAL_ID:
      return getPersistentResource_2011Text(view);
    case DiagramArcEditPart.VISUAL_ID:
      return getDiagramArc_3001Text(view);
    }
    return getUnknownElementText(view);
  }

  /**
   * @generated
   */
  private String getWorkflowDiagram_79Text(View view) {
    WorkflowDiagram domainModelElement = (WorkflowDiagram) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getName();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 79); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getStartNode_1001Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.StartNode_1001, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(StartNodeLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4001); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getEndnode_1002Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Endnode_1002, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(EndnodeLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4002); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getSimpleActivity_1003Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.SimpleActivity_1003,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(SimpleActivityLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4003); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getSubprocess_1004Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Subprocess_1004,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry.getType(SubprocessLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4004); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getEvent_1005Text(View view) {
    Event domainModelElement = (Event) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 1005); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getSplitNode_1006Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.SplitNode_1006, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(SplitNodeLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4005); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getMergeNode_1007Text(View view) {
    MergeNode domainModelElement = (MergeNode) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 1007); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getFlow_1008Text(View view) {
    Flow domainModelElement = (Flow) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 1008); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getTransientResource_1009Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.TransientResource_1009,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(TransientResourceLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4014); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getDocument_1010Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Document_1010, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(DocumentLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4015); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getPersistentResource_1011Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.PersistentResource_1011,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(PersistentResourceLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4016); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getStartNode_2001Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.StartNode_2001, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(StartNodeLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4006); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getEndnode_2002Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Endnode_2002, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(EndnodeLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4007); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getSimpleActivity_2003Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.SimpleActivity_2003,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(SimpleActivityLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4008); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getSubprocess_2004Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Subprocess_2004,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry.getType(SubprocessLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4009); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getEvent_2005Text(View view) {
    Event domainModelElement = (Event) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 2005); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getSplitNode_2006Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.SplitNode_2006, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(SplitNodeLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4010); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getMergeNode_2007Text(View view) {
    MergeNode domainModelElement = (MergeNode) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 2007); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getFlow_2008Text(View view) {
    Flow domainModelElement = (Flow) view.getElement();
    if (domainModelElement != null) {
      return domainModelElement.getLabel();
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "No domain element for view with visualID = " + 2008); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }
  }

  /**
   * @generated
   */
  private String getTransientResource_2009Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.TransientResource_2009,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(TransientResourceLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4011); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getDocument_2010Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.Document_2010, (view.getElement() != null ? view
            .getElement() : view), FlowchartVisualIDRegistry
            .getType(DocumentLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4012); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getPersistentResource_2011Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.PersistentResource_2011,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry
            .getType(PersistentResourceLabel2EditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4013); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getDiagramArc_3001Text(View view) {
    IAdaptable hintAdapter = new FlowchartParserProvider.HintAdapter(
        FlowchartElementTypes.DiagramArc_3001,
        (view.getElement() != null ? view.getElement() : view),
        FlowchartVisualIDRegistry.getType(DiagramArcLabelEditPart.VISUAL_ID));
    IParser parser = ParserService.getInstance().getParser(hintAdapter);

    if (parser != null) {
      return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
    } else {
      FlowchartDiagramEditorPlugin.getInstance().logError(
          "Parser was not found for label " + 4017); //$NON-NLS-1$
      return ""; //$NON-NLS-1$
    }

  }

  /**
   * @generated
   */
  private String getUnknownElementText(View view) {
    return "<UnknownElement Visual_ID = " + view.getType() + ">"; //$NON-NLS-1$ //$NON-NLS-2$
  }

  /**
   * @generated
   */
  private String getUnresolvedDomainElementProxyText(View view) {
    return "<Unresolved domain element Visual_ID = " + view.getType() + ">"; //$NON-NLS-1$ //$NON-NLS-2$
  }

  /**
   * @generated
   */
  public void init(ICommonContentExtensionSite aConfig) {
  }

  /**
   * @generated
   */
  public void restoreState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public void saveState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public String getDescription(Object anElement) {
    return null;
  }

  /**
   * @generated
   */
  private boolean isOwnView(View view) {
    return WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
        .getModelID(view));
  }

}
