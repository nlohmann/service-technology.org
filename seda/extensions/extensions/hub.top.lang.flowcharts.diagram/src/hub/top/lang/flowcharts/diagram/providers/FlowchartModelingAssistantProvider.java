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
package hub.top.lang.flowcharts.diagram.providers;

import hub.top.lang.flowcharts.diagram.edit.parts.FlowFlowCompartment2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowFlowCompartmentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;
import hub.top.lang.flowcharts.diagram.part.Messages;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.emf.type.core.ElementTypeRegistry;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.ui.services.modelingassistant.ModelingAssistantProvider;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;

/**
 * @generated
 */
public class FlowchartModelingAssistantProvider extends
    ModelingAssistantProvider {

  /**
   * @generated
   */
  public List getTypesForPopupBar(IAdaptable host) {
    IGraphicalEditPart editPart = (IGraphicalEditPart) host
        .getAdapter(IGraphicalEditPart.class);
    if (editPart instanceof FlowFlowCompartmentEditPart) {
      List types = new ArrayList();
      types.add(FlowchartElementTypes.StartNode_2001);
      types.add(FlowchartElementTypes.Endnode_2002);
      types.add(FlowchartElementTypes.SimpleActivity_2003);
      types.add(FlowchartElementTypes.Subprocess_2004);
      types.add(FlowchartElementTypes.Event_2005);
      types.add(FlowchartElementTypes.SplitNode_2006);
      types.add(FlowchartElementTypes.MergeNode_2007);
      types.add(FlowchartElementTypes.Flow_2008);
      types.add(FlowchartElementTypes.TransientResource_2009);
      types.add(FlowchartElementTypes.Document_2010);
      types.add(FlowchartElementTypes.PersistentResource_2011);
      return types;
    }
    if (editPart instanceof FlowFlowCompartment2EditPart) {
      List types = new ArrayList();
      types.add(FlowchartElementTypes.StartNode_2001);
      types.add(FlowchartElementTypes.Endnode_2002);
      types.add(FlowchartElementTypes.SimpleActivity_2003);
      types.add(FlowchartElementTypes.Subprocess_2004);
      types.add(FlowchartElementTypes.Event_2005);
      types.add(FlowchartElementTypes.SplitNode_2006);
      types.add(FlowchartElementTypes.MergeNode_2007);
      types.add(FlowchartElementTypes.Flow_2008);
      types.add(FlowchartElementTypes.TransientResource_2009);
      types.add(FlowchartElementTypes.Document_2010);
      types.add(FlowchartElementTypes.PersistentResource_2011);
      return types;
    }
    if (editPart instanceof WorkflowDiagramEditPart) {
      List types = new ArrayList();
      types.add(FlowchartElementTypes.StartNode_1001);
      types.add(FlowchartElementTypes.Endnode_1002);
      types.add(FlowchartElementTypes.SimpleActivity_1003);
      types.add(FlowchartElementTypes.Subprocess_1004);
      types.add(FlowchartElementTypes.Event_1005);
      types.add(FlowchartElementTypes.SplitNode_1006);
      types.add(FlowchartElementTypes.MergeNode_1007);
      types.add(FlowchartElementTypes.Flow_1008);
      types.add(FlowchartElementTypes.TransientResource_1009);
      types.add(FlowchartElementTypes.Document_1010);
      types.add(FlowchartElementTypes.PersistentResource_1011);
      return types;
    }
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public List getRelTypesOnSource(IAdaptable source) {
    IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
        .getAdapter(IGraphicalEditPart.class);
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public List getRelTypesOnTarget(IAdaptable target) {
    IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
        .getAdapter(IGraphicalEditPart.class);
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public List getRelTypesOnSourceAndTarget(IAdaptable source, IAdaptable target) {
    IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
        .getAdapter(IGraphicalEditPart.class);
    IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
        .getAdapter(IGraphicalEditPart.class);
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public List getTypesForSource(IAdaptable target, IElementType relationshipType) {
    IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
        .getAdapter(IGraphicalEditPart.class);
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public List getTypesForTarget(IAdaptable source, IElementType relationshipType) {
    IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
        .getAdapter(IGraphicalEditPart.class);
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public EObject selectExistingElementForSource(IAdaptable target,
      IElementType relationshipType) {
    return selectExistingElement(target, getTypesForSource(target,
        relationshipType));
  }

  /**
   * @generated
   */
  public EObject selectExistingElementForTarget(IAdaptable source,
      IElementType relationshipType) {
    return selectExistingElement(source, getTypesForTarget(source,
        relationshipType));
  }

  /**
   * @generated
   */
  protected EObject selectExistingElement(IAdaptable host, Collection types) {
    if (types.isEmpty()) {
      return null;
    }
    IGraphicalEditPart editPart = (IGraphicalEditPart) host
        .getAdapter(IGraphicalEditPart.class);
    if (editPart == null) {
      return null;
    }
    Diagram diagram = (Diagram) editPart.getRoot().getContents().getModel();
    Collection elements = new HashSet();
    for (Iterator it = diagram.getElement().eAllContents(); it.hasNext();) {
      EObject element = (EObject) it.next();
      if (isApplicableElement(element, types)) {
        elements.add(element);
      }
    }
    if (elements.isEmpty()) {
      return null;
    }
    return selectElement((EObject[]) elements.toArray(new EObject[elements
        .size()]));
  }

  /**
   * @generated
   */
  protected boolean isApplicableElement(EObject element, Collection types) {
    IElementType type = ElementTypeRegistry.getInstance().getElementType(
        element);
    return types.contains(type);
  }

  /**
   * @generated
   */
  protected EObject selectElement(EObject[] elements) {
    Shell shell = Display.getCurrent().getActiveShell();
    ILabelProvider labelProvider = new AdapterFactoryLabelProvider(
        FlowchartDiagramEditorPlugin.getInstance()
            .getItemProvidersAdapterFactory());
    ElementListSelectionDialog dialog = new ElementListSelectionDialog(shell,
        labelProvider);
    dialog.setMessage(Messages.FlowchartModelingAssistantProviderMessage);
    dialog.setTitle(Messages.FlowchartModelingAssistantProviderTitle);
    dialog.setMultipleSelection(false);
    dialog.setElements(elements);
    EObject selected = null;
    if (dialog.open() == Window.OK) {
      selected = (EObject) dialog.getFirstResult();
    }
    return selected;
  }
}
