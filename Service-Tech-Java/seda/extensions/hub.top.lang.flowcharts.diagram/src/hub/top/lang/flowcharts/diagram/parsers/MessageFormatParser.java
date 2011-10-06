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

import java.text.FieldPosition;
import java.text.MessageFormat;
import java.text.ParsePosition;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.command.ICommand;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParserEditStatus;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserEditStatus;
import org.eclipse.osgi.util.NLS;

/**
 * @generated
 */
public class MessageFormatParser extends AbstractParser {

  /**
   * @generated
   */
  private String defaultPattern;

  /**
   * @generated
   */
  private MessageFormat viewProcessor;

  /**
   * @generated
   */
  private MessageFormat editorProcessor;

  /**
   * @generated
   */
  private MessageFormat editProcessor;

  /**
   * @generated
   */
  public MessageFormatParser(EAttribute[] features) {
    super(features);
  }

  /**
   * @generated
   */
  protected String getDefaultPattern() {
    if (defaultPattern == null) {
      StringBuffer sb = new StringBuffer();
      for (int i = 0; i < features.length; i++) {
        if (i > 0) {
          sb.append(' ');
        }
        sb.append('{');
        sb.append(i);
        sb.append('}');
      }
      defaultPattern = sb.toString();
    }
    return defaultPattern;
  }

  /**
   * @generated
   */
  public String getViewPattern() {
    String pattern = super.getViewPattern();
    return pattern != null ? pattern : getDefaultPattern();
  }

  /**
   * @generated
   */
  public void setViewPattern(String viewPattern) {
    super.setViewPattern(viewPattern);
    viewProcessor = null;
  }

  /**
   * @generated
   */
  protected MessageFormat createViewProcessor(String viewPattern) {
    return new MessageFormat(viewPattern);
  }

  /**
   * @generated
   */
  protected MessageFormat getViewProcessor() {
    if (viewProcessor == null) {
      viewProcessor = createViewProcessor(getViewPattern());
    }
    return viewProcessor;
  }

  /**
   * @generated
   */
  public String getEditorPattern() {
    String pattern = super.getEditorPattern();
    return pattern != null ? pattern : getDefaultPattern();
  }

  /**
   * @generated
   */
  public void setEditorPattern(String editorPattern) {
    super.setEditorPattern(editorPattern);
    editorProcessor = null;
  }

  /**
   * @generated
   */
  protected MessageFormat createEditorProcessor(String editorPattern) {
    return new MessageFormat(editorPattern);
  }

  /**
   * @generated
   */
  protected MessageFormat getEditorProcessor() {
    if (editorProcessor == null) {
      editorProcessor = createEditorProcessor(getEditorPattern());
    }
    return editorProcessor;
  }

  /**
   * @generated
   */
  public String getEditPattern() {
    String pattern = super.getEditPattern();
    return pattern != null ? pattern : getDefaultPattern();
  }

  /**
   * @generated
   */
  public void setEditPattern(String editPattern) {
    super.setEditPattern(editPattern);
    editProcessor = null;
  }

  /**
   * @generated
   */
  protected MessageFormat createEditProcessor(String editPattern) {
    return new MessageFormat(editPattern);
  }

  /**
   * @generated
   */
  protected MessageFormat getEditProcessor() {
    if (editProcessor == null) {
      editProcessor = createEditProcessor(getEditPattern());
    }
    return editProcessor;
  }

  /**
   * @generated
   */
  public String getPrintString(IAdaptable adapter, int flags) {
    EObject element = (EObject) adapter.getAdapter(EObject.class);
    return getViewProcessor().format(getValues(element), new StringBuffer(),
        new FieldPosition(0)).toString();
  }

  /**
   * @generated
   */
  public String getEditString(IAdaptable adapter, int flags) {
    EObject element = (EObject) adapter.getAdapter(EObject.class);
    return getEditorProcessor().format(getValues(element), new StringBuffer(),
        new FieldPosition(0)).toString();
  }

  /**
   * @generated
   */
  public IParserEditStatus isValidEditString(IAdaptable adapter,
      String editString) {
    ParsePosition pos = new ParsePosition(0);
    Object[] values = getEditProcessor().parse(editString, pos);
    if (values == null) {
      return new ParserEditStatus(FlowchartDiagramEditorPlugin.ID,
          IParserEditStatus.UNEDITABLE, NLS.bind(
              Messages.MessageFormatParser_InvalidInputError, new Integer(pos
                  .getErrorIndex())));
    }
    return validateNewValues(values);
  }

  /**
   * @generated
   */
  public ICommand getParseCommand(IAdaptable adapter, String newString,
      int flags) {
    Object[] values = getEditProcessor().parse(newString, new ParsePosition(0));
    return getParseCommand(adapter, values, flags);
  }
}
