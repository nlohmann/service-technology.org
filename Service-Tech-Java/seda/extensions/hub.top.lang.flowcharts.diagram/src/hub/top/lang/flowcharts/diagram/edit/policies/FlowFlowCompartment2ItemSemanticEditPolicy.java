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
package hub.top.lang.flowcharts.diagram.edit.policies;

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.diagram.edit.commands.Document2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.Endnode2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.Event2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.Flow2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.MergeNode2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.PersistentResource2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.SimpleActivity2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.SplitNode2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.StartNode2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.Subprocess2CreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.TransientResource2CreateCommand;
import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;

import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;

/**
 * @generated
 */
public class FlowFlowCompartment2ItemSemanticEditPolicy extends
    FlowchartBaseItemSemanticEditPolicy {

  /**
   * @generated
   */
  protected Command getCreateCommand(CreateElementRequest req) {
    if (FlowchartElementTypes.StartNode_3001 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new StartNode2CreateCommand(req));
    }
    if (FlowchartElementTypes.Endnode_3002 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new Endnode2CreateCommand(req));
    }
    if (FlowchartElementTypes.SimpleActivity_3003 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new SimpleActivity2CreateCommand(req));
    }
    if (FlowchartElementTypes.Subprocess_3004 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new Subprocess2CreateCommand(req));
    }
    if (FlowchartElementTypes.Event_3005 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new Event2CreateCommand(req));
    }
    if (FlowchartElementTypes.SplitNode_3006 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new SplitNode2CreateCommand(req));
    }
    if (FlowchartElementTypes.MergeNode_3007 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new MergeNode2CreateCommand(req));
    }
    if (FlowchartElementTypes.Flow_3008 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new Flow2CreateCommand(req));
    }
    if (FlowchartElementTypes.TransientResource_3009 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new TransientResource2CreateCommand(req));
    }
    if (FlowchartElementTypes.Document_3010 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new Document2CreateCommand(req));
    }
    if (FlowchartElementTypes.PersistentResource_3011 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getStructuredNode_ChildNodes());
      }
      return getGEFWrapper(new PersistentResource2CreateCommand(req));
    }
    return super.getCreateCommand(req);
  }

}
