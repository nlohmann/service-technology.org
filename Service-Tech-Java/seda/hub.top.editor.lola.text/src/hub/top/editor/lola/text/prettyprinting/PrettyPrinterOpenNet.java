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
import hub.top.editor.ptnetLoLA.NodeType;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.RefMarkedPlace;
import hub.top.editor.ptnetLoLA.Transition;

public class PrettyPrinterOpenNet extends hub.sam.tef.prettyprinting.PrettyPrinter {
	
	//private ISemanticsProvider 	fSemanticsProvider;
	//private Syntax				fSyntax;

	/**
	 * @param syntax
	 * @param semanticsProvider
	 */
	public PrettyPrinterOpenNet(Syntax syntax, ISemanticsProvider semanticsProvider) {
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
	
	/**
	 * Convert non-LoLA names to LoLA names
	 * @param s
	 * @return
	 */
	private String autoFormatIdent(String s) {
		//String s2 = s.replace(' ', '_');
		//return s2;
		return s;
	}

	@Override
	public PrettyPrintState print(EObject root) throws ModelCreatingException {
		//System.err.println("print open net root");
		getLayoutManager().setup();
		PrettyPrintState state = new PrettyPrintState(root);

		if (root instanceof PtNet) {
			PtNet net = (PtNet) root;
			state.append("PLACE");
			state.append(getWhiteSpace("blockstart"));
			
			state.append(getWhiteSpace("indent")+"INTERNAL");
			state.append(getWhiteSpace("blockstart"));
			boolean first = true;
			for (Place p : net.getPlaces()) {
				if (p.getType() == NodeType.INTERNAL) {
					if (!first)	state.append(", ");
					if (first) state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(p.getName()));
					first = false;
				}
			}
			state.append(";");
			state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
			
			state.append(getWhiteSpace("indent")+"INPUT");
			state.append(getWhiteSpace("blockstart"));
			first = true;
			for (Place p : net.getPlaces()) {
				if (p.getType() == NodeType.INPUT) {
					if (!first) state.append(","+getWhiteSpace("statement"));
					state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(p.getName()));
					first = false;
				}
			}
			state.append(";");
			state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
			
			state.append(getWhiteSpace("indent")+"OUTPUT");
			state.append(getWhiteSpace("blockstart"));
			first = true;
			for (Place p : net.getPlaces()) {
				if (p.getType() == NodeType.OUTPUT) {
					if (!first) state.append(","+getWhiteSpace("statement"));
					state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(p.getName()));
					first = false;
				}
			}
			state.append(";");
			state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
			state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
			
			state.append("INITIALMARKING");
			state.append(getWhiteSpace("blockstart"));
			first = true;
			for (RefMarkedPlace pRef : net.getInitialMarking().getPlaces()) {
				if (!first)	state.append(", ");
				if (first) state.append(getWhiteSpace("indent"));
				state.append(autoFormatIdent(pRef.getPlace().getName())+":"+pRef.getToken());
				first = false;
			}
			state.append(";");
			state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));

			if (net.getFinalMarking() != null) {
				state.append("FINALMARKING");
				state.append(getWhiteSpace("blockstart"));
				first = true;
				for (RefMarkedPlace pRef : net.getFinalMarking().getPlaces()) {
					if (!first)	state.append(", ");
					if (first) state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(pRef.getPlace().getName())+":"+pRef.getToken());
					first = false;
				}
				state.append(";");
				state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
			}
			state.append(getWhiteSpace("statement"));

			for (Transition t : net.getTransitions()) {
				state.append("TRANSITION "+autoFormatIdent(t.getName()));
				state.append(getWhiteSpace("statement"));
				
				state.append("CONSUME");
				state.append(getWhiteSpace("blockstart"));
				first = true;
				for (Arc arc : t.getIncoming()) {
					if (!first) state.append(","+getWhiteSpace("statement"));
					state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(arc.getSource().getName())+":"+arc.getWeight());
					first = false;
				}
				state.append(";");
				state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));

				state.append("PRODUCE");
				state.append(getWhiteSpace("blockstart"));
				first = true;
				for (Arc arc : t.getOutgoing()) {
					if (!first) state.append(","+getWhiteSpace("statement"));
					state.append(getWhiteSpace("indent"));
					state.append(autoFormatIdent(arc.getTarget().getName())+":"+arc.getWeight());
					first = false;
				}
				state.append(";");
				state.append(getWhiteSpace("blockend")+getWhiteSpace("statement"));
				state.append(getWhiteSpace("statement"));
			}
		}
		
		return state;
	}
}
