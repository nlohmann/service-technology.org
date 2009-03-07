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

package hub.top.editor.lola.text.prettyprinting;

import org.eclipse.emf.ecore.EObject;

import hub.sam.tef.layout.UnknownWhitespaceRole;
import hub.sam.tef.modelcreating.ModelCreatingException;
import hub.sam.tef.prettyprinting.PrettyPrintState;
import hub.sam.tef.semantics.ISemanticsProvider;
import hub.sam.tef.tsl.Syntax;
import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.Transition;

public class PrettyPrinterTPN extends hub.sam.tef.prettyprinting.PrettyPrinter {
	
	//private ISemanticsProvider 	fSemanticsProvider;
	//private Syntax				fSyntax;

	/**
	 * @param syntax
	 * @param semanticsProvider
	 */
	public PrettyPrinterTPN(Syntax syntax, ISemanticsProvider semanticsProvider) {
		super(syntax, semanticsProvider);
		//fSemanticsProvider = semanticsProvider;
		//fSyntax = syntax;
	}

	private String getWhiteSpace(String role) {
		try {
			return getLayoutManager().getSpace(role);
		} catch (UnknownWhitespaceRole ex) {
			return " ";
		}
	}

	@Override
	public PrettyPrintState print(EObject root) throws ModelCreatingException {
		//System.err.println("print open net root");
		getLayoutManager().setup();
		PrettyPrintState state = new PrettyPrintState(root);

		if (root instanceof PtNet) {
			PtNet net = (PtNet) root;

			for (Place p : net.getPlaces()) {
				state.append("place"+getWhiteSpace("ws")+"\""+p.getName()+"\"");
				if (p.getToken() > 0)
					state.append(getWhiteSpace("ws")+"init"+getWhiteSpace("ws")+p.getToken());
				state.append(";"+getWhiteSpace("statement"));
			}
			state.append(getWhiteSpace("statement"));

			for (Transition t : net.getTransitions()) {
				state.append("trans"+getWhiteSpace("ws")+"\""+t.getName()+"\"");
				state.append(getWhiteSpace("statement"));
				
				state.append("in");
				state.append(getWhiteSpace("blockstart"));
				for (Arc arc : t.getIncoming()) {
					state.append(getWhiteSpace("indent")+"\""+arc.getSource().getName()+"\""+getWhiteSpace("statement"));
				}
				state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));

				state.append("out");
				state.append(getWhiteSpace("blockstart"));
				for (Arc arc : t.getOutgoing()) {
					state.append(getWhiteSpace("indent")+"\""+arc.getTarget().getName()+"\""+getWhiteSpace("statement"));
				}
				state.append(";");
				state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
				state.append(getWhiteSpace("statement"));
			}
		}
		
		return state;
	}
}
