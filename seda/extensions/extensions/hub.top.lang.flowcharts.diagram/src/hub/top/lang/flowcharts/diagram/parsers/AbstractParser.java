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
package hub.top.lang.flowcharts.diagram.parsers;

import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;
import hub.top.lang.flowcharts.diagram.part.Messages;

import java.util.Arrays;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClassifier;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EEnumLiteral;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.gmf.runtime.common.core.command.ICommand;
import org.eclipse.gmf.runtime.common.core.command.UnexecutableCommand;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParserEditStatus;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserEditStatus;
import org.eclipse.gmf.runtime.emf.commands.core.command.CompositeTransactionalCommand;
import org.eclipse.gmf.runtime.emf.type.core.commands.SetValueCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.SetRequest;
import org.eclipse.jface.text.contentassist.IContentAssistProcessor;
import org.eclipse.osgi.util.NLS;

/**
 * @generated
 */
public abstract class AbstractParser implements IParser {

  /**
   * @generated
   */
  protected final EAttribute[] features;

  /**
   * @generated
   */
  private String viewPattern;

  /**
   * @generated
   */
  private String editorPattern;

  /**
   * @generated
   */
  private String editPattern;

  /**
   * @generated
   */
  public AbstractParser(EAttribute[] features) {
    if (features == null || Arrays.asList(features).contains(null)) {
      throw new IllegalArgumentException();
    }
    this.features = features;
  }

  /**
   * @generated
   */
  public String getViewPattern() {
    return viewPattern;
  }

  /**
   * @generated
   */
  public void setViewPattern(String viewPattern) {
    this.viewPattern = viewPattern;
  }

  /**
   * @generated
   */
  public String getEditorPattern() {
    return editorPattern;
  }

  /**
   * @generated
   */
  public void setEditorPattern(String editorPattern) {
    this.editorPattern = editorPattern;
  }

  /**
   * @generated
   */
  public String getEditPattern() {
    return editPattern;
  }

  /**
   * @generated
   */
  public void setEditPattern(String editPattern) {
    this.editPattern = editPattern;
  }

  /**
   * @generated
   */
  public boolean isAffectingEvent(Object event, int flags) {
    if (event instanceof Notification) {
      return isAffectingFeature(((Notification) event).getFeature());
    }
    return false;
  }

  /**
   * @generated
   */
  protected boolean isAffectingFeature(Object feature) {
    for (int i = 0; i < features.length; i++) {
      if (features[i] == feature) {
        return true;
      }
    }
    return false;
  }

  /**
   * @generated
   */
  public IContentAssistProcessor getCompletionProcessor(IAdaptable element) {
    return null;
  }

  /**
   * @generated
   */
  protected Object[] getValues(EObject element) {
    Object[] values = new Object[features.length];
    for (int i = 0; i < features.length; i++) {
      values[i] = getValue(element, features[i]);
    }
    return values;
  }

  /**
   * @generated
   */
  protected Object getValue(EObject element, EAttribute feature) {
    Object value = element.eGet(feature);
    Class iClass = feature.getEAttributeType().getInstanceClass();
    if (String.class.equals(iClass)) {
      if (value == null) {
        value = ""; //$NON-NLS-1$
      }
    }
    return value;
  }

  /**
   * @generated
   */
  protected ICommand getParseCommand(IAdaptable adapter, Object[] values,
      int flags) {
    if (values == null
        || validateNewValues(values).getCode() != IParserEditStatus.EDITABLE) {
      return UnexecutableCommand.INSTANCE;
    }
    EObject element = (EObject) adapter.getAdapter(EObject.class);
    TransactionalEditingDomain editingDomain = TransactionUtil
        .getEditingDomain(element);
    if (editingDomain == null) {
      return UnexecutableCommand.INSTANCE;
    }
    CompositeTransactionalCommand command = new CompositeTransactionalCommand(
        editingDomain, "Set Values"); //$NON-NLS-1$
    for (int i = 0; i < values.length; i++) {
      command.compose(getModificationCommand(element, features[i], values[i]));
    }
    return command;
  }

  /**
   * @generated
   */
  protected ICommand getModificationCommand(EObject element,
      EAttribute feature, Object value) {
    value = getValidNewValue(feature, value);
    if (value instanceof InvalidValue) {
      return UnexecutableCommand.INSTANCE;
    }
    SetRequest request = new SetRequest(element, feature, value);
    return new SetValueCommand(request);
  }

  /**
   * @generated
   */
  protected IParserEditStatus validateNewValues(Object[] values) {
    if (values.length != features.length) {
      return ParserEditStatus.UNEDITABLE_STATUS;
    }
    for (int i = 0; i < values.length; i++) {
      Object value = getValidNewValue(features[i], values[i]);
      if (value instanceof InvalidValue) {
        return new ParserEditStatus(FlowchartDiagramEditorPlugin.ID,
            IParserEditStatus.UNEDITABLE, value.toString());
      }
    }
    return ParserEditStatus.EDITABLE_STATUS;
  }

  /**
   * @generated
   */
  protected Object getValidNewValue(EAttribute feature, Object value) {
    EClassifier type = feature.getEType();
    if (type instanceof EDataType) {
      Class iClass = type.getInstanceClass();
      if (Boolean.TYPE.equals(iClass)) {
        if (value instanceof Boolean) {
          // ok
        } else if (value instanceof String) {
          value = Boolean.valueOf((String) value);
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Character.TYPE.equals(iClass)) {
        if (value instanceof Character) {
          // ok
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            value = new Character(s.charAt(0));
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Byte.TYPE.equals(iClass)) {
        if (value instanceof Byte) {
          // ok
        } else if (value instanceof Number) {
          value = new Byte(((Number) value).byteValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Byte.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Short.TYPE.equals(iClass)) {
        if (value instanceof Short) {
          // ok
        } else if (value instanceof Number) {
          value = new Short(((Number) value).shortValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Short.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Integer.TYPE.equals(iClass)) {
        if (value instanceof Integer) {
          // ok
        } else if (value instanceof Number) {
          value = new Integer(((Number) value).intValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Integer.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Long.TYPE.equals(iClass)) {
        if (value instanceof Long) {
          // ok
        } else if (value instanceof Number) {
          value = new Long(((Number) value).longValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Long.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Float.TYPE.equals(iClass)) {
        if (value instanceof Float) {
          // ok
        } else if (value instanceof Number) {
          value = new Float(((Number) value).floatValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Float.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (Double.TYPE.equals(iClass)) {
        if (value instanceof Double) {
          // ok
        } else if (value instanceof Number) {
          value = new Double(((Number) value).doubleValue());
        } else if (value instanceof String) {
          String s = (String) value;
          if (s.length() == 0) {
            value = null;
          } else {
            try {
              value = Double.valueOf(s);
            } catch (NumberFormatException nfe) {
              value = new InvalidValue(NLS.bind(
                  Messages.AbstractParser_WrongStringConversionMessage, iClass
                      .getName()));
            }
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, iClass
                  .getName()));
        }
      } else if (type instanceof EEnum) {
        if (value instanceof String) {
          EEnumLiteral literal = ((EEnum) type)
              .getEEnumLiteralByLiteral((String) value);
          if (literal == null) {
            value = new InvalidValue(NLS.bind(
                Messages.AbstractParser_UnknownLiteralMessage, value));
          } else {
            value = literal.getInstance();
          }
        } else {
          value = new InvalidValue(NLS.bind(
              Messages.AbstractParser_UnexpectedValueTypeMessage, String.class
                  .getName()));
        }
      }
    }
    return value;
  }

  /**
   * @generated
   */
  protected class InvalidValue {

    /**
     * @generated
     */
    private String description;

    /**
     * @generated
     */
    public InvalidValue(String description) {
      this.description = description;
    }

    /**
     * @generated
     */
    public String toString() {
      return description;
    }
  }
}
