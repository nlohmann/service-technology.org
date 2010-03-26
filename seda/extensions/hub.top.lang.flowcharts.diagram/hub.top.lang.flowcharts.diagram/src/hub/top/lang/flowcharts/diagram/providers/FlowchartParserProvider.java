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

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.parsers.MessageFormatParser;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.service.AbstractProvider;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.GetParserOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParserProvider;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.ui.services.parser.ParserHintAdapter;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartParserProvider extends AbstractProvider implements
    IParserProvider {

  /**
   * @generated
   */
  private IParser startNodeLabel_4001Parser;

  /**
   * @generated
   */
  private IParser getStartNodeLabel_4001Parser() {
    if (startNodeLabel_4001Parser == null) {
      startNodeLabel_4001Parser = createStartNodeLabel_4001Parser();
    }
    return startNodeLabel_4001Parser;
  }

  /**
   * @generated
   */
  protected IParser createStartNodeLabel_4001Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser endnodeLabel_4002Parser;

  /**
   * @generated
   */
  private IParser getEndnodeLabel_4002Parser() {
    if (endnodeLabel_4002Parser == null) {
      endnodeLabel_4002Parser = createEndnodeLabel_4002Parser();
    }
    return endnodeLabel_4002Parser;
  }

  /**
   * @generated
   */
  protected IParser createEndnodeLabel_4002Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser simpleActivityLabel_4003Parser;

  /**
   * @generated
   */
  private IParser getSimpleActivityLabel_4003Parser() {
    if (simpleActivityLabel_4003Parser == null) {
      simpleActivityLabel_4003Parser = createSimpleActivityLabel_4003Parser();
    }
    return simpleActivityLabel_4003Parser;
  }

  /**
   * @generated
   */
  protected IParser createSimpleActivityLabel_4003Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser subprocessLabel_4004Parser;

  /**
   * @generated
   */
  private IParser getSubprocessLabel_4004Parser() {
    if (subprocessLabel_4004Parser == null) {
      subprocessLabel_4004Parser = createSubprocessLabel_4004Parser();
    }
    return subprocessLabel_4004Parser;
  }

  /**
   * @generated
   */
  protected IParser createSubprocessLabel_4004Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser splitNodeLabel_4005Parser;

  /**
   * @generated
   */
  private IParser getSplitNodeLabel_4005Parser() {
    if (splitNodeLabel_4005Parser == null) {
      splitNodeLabel_4005Parser = createSplitNodeLabel_4005Parser();
    }
    return splitNodeLabel_4005Parser;
  }

  /**
   * @generated
   */
  protected IParser createSplitNodeLabel_4005Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser transientResourceLabel_4014Parser;

  /**
   * @generated
   */
  private IParser getTransientResourceLabel_4014Parser() {
    if (transientResourceLabel_4014Parser == null) {
      transientResourceLabel_4014Parser = createTransientResourceLabel_4014Parser();
    }
    return transientResourceLabel_4014Parser;
  }

  /**
   * @generated
   */
  protected IParser createTransientResourceLabel_4014Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser documentLabel_4015Parser;

  /**
   * @generated
   */
  private IParser getDocumentLabel_4015Parser() {
    if (documentLabel_4015Parser == null) {
      documentLabel_4015Parser = createDocumentLabel_4015Parser();
    }
    return documentLabel_4015Parser;
  }

  /**
   * @generated
   */
  protected IParser createDocumentLabel_4015Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser persistentResourceLabel_4016Parser;

  /**
   * @generated
   */
  private IParser getPersistentResourceLabel_4016Parser() {
    if (persistentResourceLabel_4016Parser == null) {
      persistentResourceLabel_4016Parser = createPersistentResourceLabel_4016Parser();
    }
    return persistentResourceLabel_4016Parser;
  }

  /**
   * @generated
   */
  protected IParser createPersistentResourceLabel_4016Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser startNodeLabel_4006Parser;

  /**
   * @generated
   */
  private IParser getStartNodeLabel_4006Parser() {
    if (startNodeLabel_4006Parser == null) {
      startNodeLabel_4006Parser = createStartNodeLabel_4006Parser();
    }
    return startNodeLabel_4006Parser;
  }

  /**
   * @generated
   */
  protected IParser createStartNodeLabel_4006Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser endnodeLabel_4007Parser;

  /**
   * @generated
   */
  private IParser getEndnodeLabel_4007Parser() {
    if (endnodeLabel_4007Parser == null) {
      endnodeLabel_4007Parser = createEndnodeLabel_4007Parser();
    }
    return endnodeLabel_4007Parser;
  }

  /**
   * @generated
   */
  protected IParser createEndnodeLabel_4007Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser simpleActivityLabel_4008Parser;

  /**
   * @generated
   */
  private IParser getSimpleActivityLabel_4008Parser() {
    if (simpleActivityLabel_4008Parser == null) {
      simpleActivityLabel_4008Parser = createSimpleActivityLabel_4008Parser();
    }
    return simpleActivityLabel_4008Parser;
  }

  /**
   * @generated
   */
  protected IParser createSimpleActivityLabel_4008Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser subprocessLabel_4009Parser;

  /**
   * @generated
   */
  private IParser getSubprocessLabel_4009Parser() {
    if (subprocessLabel_4009Parser == null) {
      subprocessLabel_4009Parser = createSubprocessLabel_4009Parser();
    }
    return subprocessLabel_4009Parser;
  }

  /**
   * @generated
   */
  protected IParser createSubprocessLabel_4009Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser splitNodeLabel_4010Parser;

  /**
   * @generated
   */
  private IParser getSplitNodeLabel_4010Parser() {
    if (splitNodeLabel_4010Parser == null) {
      splitNodeLabel_4010Parser = createSplitNodeLabel_4010Parser();
    }
    return splitNodeLabel_4010Parser;
  }

  /**
   * @generated
   */
  protected IParser createSplitNodeLabel_4010Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser transientResourceLabel_4011Parser;

  /**
   * @generated
   */
  private IParser getTransientResourceLabel_4011Parser() {
    if (transientResourceLabel_4011Parser == null) {
      transientResourceLabel_4011Parser = createTransientResourceLabel_4011Parser();
    }
    return transientResourceLabel_4011Parser;
  }

  /**
   * @generated
   */
  protected IParser createTransientResourceLabel_4011Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser documentLabel_4012Parser;

  /**
   * @generated
   */
  private IParser getDocumentLabel_4012Parser() {
    if (documentLabel_4012Parser == null) {
      documentLabel_4012Parser = createDocumentLabel_4012Parser();
    }
    return documentLabel_4012Parser;
  }

  /**
   * @generated
   */
  protected IParser createDocumentLabel_4012Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser persistentResourceLabel_4013Parser;

  /**
   * @generated
   */
  private IParser getPersistentResourceLabel_4013Parser() {
    if (persistentResourceLabel_4013Parser == null) {
      persistentResourceLabel_4013Parser = createPersistentResourceLabel_4013Parser();
    }
    return persistentResourceLabel_4013Parser;
  }

  /**
   * @generated
   */
  protected IParser createPersistentResourceLabel_4013Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser diagramArcLabel_4017Parser;

  /**
   * @generated
   */
  private IParser getDiagramArcLabel_4017Parser() {
    if (diagramArcLabel_4017Parser == null) {
      diagramArcLabel_4017Parser = createDiagramArcLabel_4017Parser();
    }
    return diagramArcLabel_4017Parser;
  }

  /**
   * @generated
   */
  protected IParser createDiagramArcLabel_4017Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramArc_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  protected IParser getParser(int visualID) {
    switch (visualID) {
    case StartNodeLabelEditPart.VISUAL_ID:
      return getStartNodeLabel_4001Parser();
    case EndnodeLabelEditPart.VISUAL_ID:
      return getEndnodeLabel_4002Parser();
    case SimpleActivityLabelEditPart.VISUAL_ID:
      return getSimpleActivityLabel_4003Parser();
    case SubprocessLabelEditPart.VISUAL_ID:
      return getSubprocessLabel_4004Parser();
    case SplitNodeLabelEditPart.VISUAL_ID:
      return getSplitNodeLabel_4005Parser();
    case TransientResourceLabelEditPart.VISUAL_ID:
      return getTransientResourceLabel_4014Parser();
    case DocumentLabelEditPart.VISUAL_ID:
      return getDocumentLabel_4015Parser();
    case PersistentResourceLabelEditPart.VISUAL_ID:
      return getPersistentResourceLabel_4016Parser();
    case StartNodeLabel2EditPart.VISUAL_ID:
      return getStartNodeLabel_4006Parser();
    case EndnodeLabel2EditPart.VISUAL_ID:
      return getEndnodeLabel_4007Parser();
    case SimpleActivityLabel2EditPart.VISUAL_ID:
      return getSimpleActivityLabel_4008Parser();
    case SubprocessLabel2EditPart.VISUAL_ID:
      return getSubprocessLabel_4009Parser();
    case SplitNodeLabel2EditPart.VISUAL_ID:
      return getSplitNodeLabel_4010Parser();
    case TransientResourceLabel2EditPart.VISUAL_ID:
      return getTransientResourceLabel_4011Parser();
    case DocumentLabel2EditPart.VISUAL_ID:
      return getDocumentLabel_4012Parser();
    case PersistentResourceLabel2EditPart.VISUAL_ID:
      return getPersistentResourceLabel_4013Parser();
    case DiagramArcLabelEditPart.VISUAL_ID:
      return getDiagramArcLabel_4017Parser();
    }
    return null;
  }

  /**
   * @generated
   */
  public IParser getParser(IAdaptable hint) {
    String vid = (String) hint.getAdapter(String.class);
    if (vid != null) {
      return getParser(FlowchartVisualIDRegistry.getVisualID(vid));
    }
    View view = (View) hint.getAdapter(View.class);
    if (view != null) {
      return getParser(FlowchartVisualIDRegistry.getVisualID(view));
    }
    return null;
  }

  /**
   * @generated
   */
  public boolean provides(IOperation operation) {
    if (operation instanceof GetParserOperation) {
      IAdaptable hint = ((GetParserOperation) operation).getHint();
      if (FlowchartElementTypes.getElement(hint) == null) {
        return false;
      }
      return getParser(hint) != null;
    }
    return false;
  }

  /**
   * @generated
   */
  public static class HintAdapter extends ParserHintAdapter {

    /**
     * @generated
     */
    private final IElementType elementType;

    /**
     * @generated
     */
    public HintAdapter(IElementType type, EObject object, String parserHint) {
      super(object, parserHint);
      assert type != null;
      elementType = type;
    }

    /**
     * @generated
     */
    public Object getAdapter(Class adapter) {
      if (IElementType.class.equals(adapter)) {
        return elementType;
      }
      return super.getAdapter(adapter);
    }
  }

}
