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

/**
 * DO NOT EDIT - Syntax generated from LolaIdent.syntax
 * at Fri Jun 13 00:51:29 CEST 2008
 * by hub.sam.tef.rcc.util.SourceGenerator.
 */

import hub.sam.tef.rcc.syntax.Rule;
import hub.sam.tef.rcc.syntax.Syntax;

public final class LolaIdentSyntax extends Syntax
{
	/**
	 * @generated
	 */
	private static final long serialVersionUID = -2610222568645677284L;
	
	public static final String Ascii = "Ascii";
	public static final String DisallowedLola = "DisallowedLola";
	public static final String lolaident = "lolaident";
	public static final String token = "token";

	public LolaIdentSyntax()	{
		super(14);

		Rule rule;

		rule = new Rule(Ascii, 3);	// 0
		rule.addRightSymbol("0x20");
		rule.addRightSymbol("..");
		rule.addRightSymbol("0x7F");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 1
		rule.addRightSymbol("','");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 2
		rule.addRightSymbol("';'");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 3
		rule.addRightSymbol("':'");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 4
		rule.addRightSymbol("'('");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 5
		rule.addRightSymbol("')'");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 6
		rule.addRightSymbol("'{'");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 7
		rule.addRightSymbol("'}'");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 8
		rule.addRightSymbol("`whitespace`");
		addRule(rule);

		rule = new Rule(lolaident, 1);	// 9
		rule.addRightSymbol("_Ascii_minus_DisallowedLola_LIST");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola_LIST", 2);	// 10
		rule.addRightSymbol("_Ascii_minus_DisallowedLola_LIST");
		rule.addRightSymbol("_Ascii_minus_DisallowedLola");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola_LIST", 1);	// 11
		rule.addRightSymbol("_Ascii_minus_DisallowedLola");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola", 3);	// 12
		rule.addRightSymbol(Ascii);
		rule.addRightSymbol("-");
		rule.addRightSymbol(DisallowedLola);
		addRule(rule);

		rule = new Rule(token, 1);	// 13
		rule.addRightSymbol(lolaident);
		addRule(rule);
	}
}
