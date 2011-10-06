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

package hub.top.editor.lola.text.syntax;

import hub.sam.tef.tokens.StandardLexerRules;
import hub.sam.tef.tokens.TokenDescriptor;

import java.util.List;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.MultiLineRule;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

public class LolaComment extends TokenDescriptor {

	@Override
	protected hub.sam.tef.rcc.syntax.Rule[] getAdditionalRCCRules() {
		// create RCC token rule according to RCC token definition
		String [][] rules = StandardLexerRules.getCustomMultiLineCommentRules(
				"lolacomment", "{", "}");
		
		hub.sam.tef.rcc.syntax.Syntax s = new hub.sam.tef.rcc.syntax.Syntax(rules);
		hub.sam.tef.rcc.syntax.Rule[] ruleSet = new hub.sam.tef.rcc.syntax.Rule[s.size()];
		for (int i=0;i<s.size();i++) {
			ruleSet[i] = s.getRule(i);
		}
		return ruleSet;
	}

	@Override
	public String getRCCSymbol() {
		return "`lolacomment`";
	}
	
	
	/**
	 * render the comment in the editor window
	 */
	@Override
	public List<IRule> getHighlightRules() {
		java.util.LinkedList<IRule> rules = new java.util.LinkedList<IRule>();
		
		rules.add(new MultiLineRule("{", "}", new Token(new TextAttribute(
				new Color(Display.getCurrent(), new RGB(0,160,0)), 
				null, SWT.ITALIC)), '\\' ));
		
		return rules;
	}
}
