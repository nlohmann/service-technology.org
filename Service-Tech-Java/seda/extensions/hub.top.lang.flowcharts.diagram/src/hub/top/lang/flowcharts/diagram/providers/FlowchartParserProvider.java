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
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcProbabilityEditPart;
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
  private IParser startNodeLabel_5001Parser;

  /**
   * @generated
   */
  private IParser getStartNodeLabel_5001Parser() {
    if (startNodeLabel_5001Parser == null) {
      startNodeLabel_5001Parser = createStartNodeLabel_5001Parser();
    }
    return startNodeLabel_5001Parser;
  }

  /**
   * @generated
   */
  protected IParser createStartNodeLabel_5001Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser endnodeLabel_5002Parser;

  /**
   * @generated
   */
  private IParser getEndnodeLabel_5002Parser() {
    if (endnodeLabel_5002Parser == null) {
      endnodeLabel_5002Parser = createEndnodeLabel_5002Parser();
    }
    return endnodeLabel_5002Parser;
  }

  /**
   * @generated
   */
  protected IParser createEndnodeLabel_5002Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser simpleActivityLabel_5003Parser;

  /**
   * @generated
   */
  private IParser getSimpleActivityLabel_5003Parser() {
    if (simpleActivityLabel_5003Parser == null) {
      simpleActivityLabel_5003Parser = createSimpleActivityLabel_5003Parser();
    }
    return simpleActivityLabel_5003Parser;
  }

  /**
   * @generated
   */
  protected IParser createSimpleActivityLabel_5003Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser subprocessLabel_5004Parser;

  /**
   * @generated
   */
  private IParser getSubprocessLabel_5004Parser() {
    if (subprocessLabel_5004Parser == null) {
      subprocessLabel_5004Parser = createSubprocessLabel_5004Parser();
    }
    return subprocessLabel_5004Parser;
  }

  /**
   * @generated
   */
  protected IParser createSubprocessLabel_5004Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser splitNodeLabel_5005Parser;

  /**
   * @generated
   */
  private IParser getSplitNodeLabel_5005Parser() {
    if (splitNodeLabel_5005Parser == null) {
      splitNodeLabel_5005Parser = createSplitNodeLabel_5005Parser();
    }
    return splitNodeLabel_5005Parser;
  }

  /**
   * @generated
   */
  protected IParser createSplitNodeLabel_5005Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser transientResourceLabel_5014Parser;

  /**
   * @generated
   */
  private IParser getTransientResourceLabel_5014Parser() {
    if (transientResourceLabel_5014Parser == null) {
      transientResourceLabel_5014Parser = createTransientResourceLabel_5014Parser();
    }
    return transientResourceLabel_5014Parser;
  }

  /**
   * @generated
   */
  protected IParser createTransientResourceLabel_5014Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser documentLabel_5015Parser;

  /**
   * @generated
   */
  private IParser getDocumentLabel_5015Parser() {
    if (documentLabel_5015Parser == null) {
      documentLabel_5015Parser = createDocumentLabel_5015Parser();
    }
    return documentLabel_5015Parser;
  }

  /**
   * @generated
   */
  protected IParser createDocumentLabel_5015Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser persistentResourceLabel_5016Parser;

  /**
   * @generated
   */
  private IParser getPersistentResourceLabel_5016Parser() {
    if (persistentResourceLabel_5016Parser == null) {
      persistentResourceLabel_5016Parser = createPersistentResourceLabel_5016Parser();
    }
    return persistentResourceLabel_5016Parser;
  }

  /**
   * @generated
   */
  protected IParser createPersistentResourceLabel_5016Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser startNodeLabel_5006Parser;

  /**
   * @generated
   */
  private IParser getStartNodeLabel_5006Parser() {
    if (startNodeLabel_5006Parser == null) {
      startNodeLabel_5006Parser = createStartNodeLabel_5006Parser();
    }
    return startNodeLabel_5006Parser;
  }

  /**
   * @generated
   */
  protected IParser createStartNodeLabel_5006Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser endnodeLabel_5007Parser;

  /**
   * @generated
   */
  private IParser getEndnodeLabel_5007Parser() {
    if (endnodeLabel_5007Parser == null) {
      endnodeLabel_5007Parser = createEndnodeLabel_5007Parser();
    }
    return endnodeLabel_5007Parser;
  }

  /**
   * @generated
   */
  protected IParser createEndnodeLabel_5007Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser simpleActivityLabel_5008Parser;

  /**
   * @generated
   */
  private IParser getSimpleActivityLabel_5008Parser() {
    if (simpleActivityLabel_5008Parser == null) {
      simpleActivityLabel_5008Parser = createSimpleActivityLabel_5008Parser();
    }
    return simpleActivityLabel_5008Parser;
  }

  /**
   * @generated
   */
  protected IParser createSimpleActivityLabel_5008Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser subprocessLabel_5009Parser;

  /**
   * @generated
   */
  private IParser getSubprocessLabel_5009Parser() {
    if (subprocessLabel_5009Parser == null) {
      subprocessLabel_5009Parser = createSubprocessLabel_5009Parser();
    }
    return subprocessLabel_5009Parser;
  }

  /**
   * @generated
   */
  protected IParser createSubprocessLabel_5009Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser splitNodeLabel_5010Parser;

  /**
   * @generated
   */
  private IParser getSplitNodeLabel_5010Parser() {
    if (splitNodeLabel_5010Parser == null) {
      splitNodeLabel_5010Parser = createSplitNodeLabel_5010Parser();
    }
    return splitNodeLabel_5010Parser;
  }

  /**
   * @generated
   */
  protected IParser createSplitNodeLabel_5010Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser transientResourceLabel_5011Parser;

  /**
   * @generated
   */
  private IParser getTransientResourceLabel_5011Parser() {
    if (transientResourceLabel_5011Parser == null) {
      transientResourceLabel_5011Parser = createTransientResourceLabel_5011Parser();
    }
    return transientResourceLabel_5011Parser;
  }

  /**
   * @generated
   */
  protected IParser createTransientResourceLabel_5011Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser documentLabel_5012Parser;

  /**
   * @generated
   */
  private IParser getDocumentLabel_5012Parser() {
    if (documentLabel_5012Parser == null) {
      documentLabel_5012Parser = createDocumentLabel_5012Parser();
    }
    return documentLabel_5012Parser;
  }

  /**
   * @generated
   */
  protected IParser createDocumentLabel_5012Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser persistentResourceLabel_5013Parser;

  /**
   * @generated
   */
  private IParser getPersistentResourceLabel_5013Parser() {
    if (persistentResourceLabel_5013Parser == null) {
      persistentResourceLabel_5013Parser = createPersistentResourceLabel_5013Parser();
    }
    return persistentResourceLabel_5013Parser;
  }

  /**
   * @generated
   */
  protected IParser createPersistentResourceLabel_5013Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramNode_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser diagramArcLabel_6001Parser;

  /**
   * @generated
   */
  private IParser getDiagramArcLabel_6001Parser() {
    if (diagramArcLabel_6001Parser == null) {
      diagramArcLabel_6001Parser = createDiagramArcLabel_6001Parser();
    }
    return diagramArcLabel_6001Parser;
  }

  /**
   * @generated
   */
  protected IParser createDiagramArcLabel_6001Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramArc_Label(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  private IParser diagramArcProbability_6002Parser;

  /**
   * @generated
   */
  private IParser getDiagramArcProbability_6002Parser() {
    if (diagramArcProbability_6002Parser == null) {
      diagramArcProbability_6002Parser = createDiagramArcProbability_6002Parser();
    }
    return diagramArcProbability_6002Parser;
  }

  /**
   * @generated
   */
  protected IParser createDiagramArcProbability_6002Parser() {
    EAttribute[] features = new EAttribute[] { FlowPackage.eINSTANCE
        .getDiagramArc_Probability(), };
    MessageFormatParser parser = new MessageFormatParser(features);
    return parser;
  }

  /**
   * @generated
   */
  protected IParser getParser(int visualID) {
    switch (visualID) {
    case StartNodeLabelEditPart.VISUAL_ID:
      return getStartNodeLabel_5001Parser();
    case EndnodeLabelEditPart.VISUAL_ID:
      return getEndnodeLabel_5002Parser();
    case SimpleActivityLabelEditPart.VISUAL_ID:
      return getSimpleActivityLabel_5003Parser();
    case SubprocessLabelEditPart.VISUAL_ID:
      return getSubprocessLabel_5004Parser();
    case SplitNodeLabelEditPart.VISUAL_ID:
      return getSplitNodeLabel_5005Parser();
    case TransientResourceLabelEditPart.VISUAL_ID:
      return getTransientResourceLabel_5014Parser();
    case DocumentLabelEditPart.VISUAL_ID:
      return getDocumentLabel_5015Parser();
    case PersistentResourceLabelEditPart.VISUAL_ID:
      return getPersistentResourceLabel_5016Parser();
    case StartNodeLabel2EditPart.VISUAL_ID:
      return getStartNodeLabel_5006Parser();
    case EndnodeLabel2EditPart.VISUAL_ID:
      return getEndnodeLabel_5007Parser();
    case SimpleActivityLabel2EditPart.VISUAL_ID:
      return getSimpleActivityLabel_5008Parser();
    case SubprocessLabel2EditPart.VISUAL_ID:
      return getSubprocessLabel_5009Parser();
    case SplitNodeLabel2EditPart.VISUAL_ID:
      return getSplitNodeLabel_5010Parser();
    case TransientResourceLabel2EditPart.VISUAL_ID:
      return getTransientResourceLabel_5011Parser();
    case DocumentLabel2EditPart.VISUAL_ID:
      return getDocumentLabel_5012Parser();
    case PersistentResourceLabel2EditPart.VISUAL_ID:
      return getPersistentResourceLabel_5013Parser();
    case DiagramArcLabelEditPart.VISUAL_ID:
      return getDiagramArcLabel_6001Parser();
    case DiagramArcProbabilityEditPart.VISUAL_ID:
      return getDiagramArcProbability_6002Parser();
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
