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

import hub.sam.tef.primitivetypes.PrimitiveTypeDescriptor;
import hub.sam.tef.rcc.syntax.Rule;
import hub.sam.tef.rcc.syntax.Syntax;
import hub.sam.tef.rcc.syntax.builder.SyntaxSeparation;

public class LolaIdent extends PrimitiveTypeDescriptor {
	
	private Syntax lexerSyntax = null;

	@Override
	public String getNonTerminalName() {
		return "LOLAIDENT";
	}

	@Override
	public String getRCCSymbol() {
		return "lolaident";
	}

	private void constructScannerRules () {
		
		try {
			/*
			// read the syntax from EBNF file
			Reader syntaxInput = new InputStreamReader(LolaIdent.class.getResourceAsStream("LolaIdent.syntax"));
			
			SyntaxBuilder sb = new SyntaxBuilder(syntaxInput);
			Syntax syntax = sb.getSyntax();
			*/
			Syntax syntax = new LolaIdentSyntax();
			SyntaxSeparation separation = new SyntaxSeparation(syntax);
			lexerSyntax = separation.getLexerSyntax();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	protected Rule[] getAdditionalRCCRules() {
		constructScannerRules();
		
		Rule[] rules = new Rule[lexerSyntax.size()];
		for (int i=0;i<lexerSyntax.size();i++)
			rules[i] = lexerSyntax.getRule(i);
		
		return rules;
	}
}
