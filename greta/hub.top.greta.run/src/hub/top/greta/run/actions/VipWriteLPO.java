/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.greta.run.actions;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.greta.simulation.AdaptiveProcessSimulationView;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class VipWriteLPO implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.export.vip.lpo";
	
	private IWorkbenchWindow workbenchWindow;

	protected AdaptiveProcessSimulationView simView = new AdaptiveProcessSimulationView();
	
	public void dispose() {
		// TODO Auto-generated method stub

	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;

		if (simView.processViewEditor == null)
			return;
		
		URI uri = simView.adaptiveSystem.eResource().getURI();
		String name = uri.lastSegment();
		String targetPath = uri.trimSegments(1).toPlatformString(true);
		
		writeLPOfile(targetPath, name, simView.adaptiveProcess);
	}

	public void selectionChanged(IAction action, ISelection selection) {
		boolean validContext = true;

		if (!action.getId().equals(ID))
			validContext = false;
		
		// set and check whether the current editor can handle this action
		simView.setProcessViewEditor_andFields(workbenchWindow.getActivePage().getActiveEditor());
		if (simView.processViewEditor == null)
			validContext = false;
		
		if (validContext) {
			if (!action.isEnabled())
				action.setEnabled(true);
		} else {
			if (action.isEnabled())
				action.setEnabled(false);

		}
	}

	private void writeLPOfile(String targetPath, String scenarioName, AdaptiveProcess ap) {

		int eventID = 0;
		
		int lpoNum = 0;
		IPath path = null;
		
		do {
			String fileName = scenarioName+"_"+lpoNum+".lpo";
			path = (new Path(targetPath)).addTrailingSeparator().append(fileName);
			if (ResourcesPlugin.getWorkspace().getRoot().exists(path)) {
				path = null;
			}
			lpoNum++;
		} while (path == null);
		
		StringBuilder b = new StringBuilder();
		
		b.append("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
		b.append("<pnml>\n");
		b.append("  <lpo id=\""+path.lastSegment()+"\" type=\"VIPschema.xsd\">\n");
		b.append("    <name><value>"+path.lastSegment()+"</value></name>\n");
		
		EMap<Event, Set<Event>> eventArcs = new BasicEMap<Event, Set<Event>>();
		Map<Event, Integer> eventIDs = new HashMap<Event, Integer>();
		
		for (Node n : ap.getNodes())
		{
			if (!(n instanceof Event))
				continue;
			
			Event e = (Event)n;
			if (e.isDisabledByAntiOclet())
				continue;
			if (e.isDisabledByConflict())
				continue;
			
			eventID++;
			
			eventIDs.put(e, eventID);
			
			b.append("    <event id=\"event"+eventID+"\" provider=\"lpo.provider.event\">\n");
			b.append("      <name>\n");
			b.append("        <value>"+e.getName().replace('&', ' ')+"</value>\n");
			b.append("        <graphics>\n");
			b.append("          <offset x=\"5\" y=\"67\"/>\n");
			b.append("          <fill color=\"rgb(255, 255, 255)\"/>\n");
			b.append("          <line color=\"rgb(0, 0, 0)\" shape=\"line\" style=\"solid\" width=\"1\"/>\n");
			b.append("          <font family=\"SansSerif\" orientation=\"0\" posture=\"0.0\" rotation=\"0.0\" size=\"10.0\" weight=\"1.0\"/>\n");
			b.append("        </graphics>\n");
			b.append("      </name>\n");
			b.append("      <graphics>\n");
			b.append("        <position x=\"105\" y=\"101\"/>\n");
			b.append("        <dimension x=\"16\" y=\"16\"/>\n");
			b.append("        <fill color=\"rgb(255, 255, 255)\"/>\n");
			b.append("        <line color=\"rgb(0, 0, 0)\" shape=\"line\" style=\"solid\" width=\"1\"/>\n");
			b.append("      </graphics>\n");
			b.append("    </event>\n");

			HashSet<Event> postEvents = new HashSet<Event>(); 
			
			// store all arcs to post-events
			for (Condition c : e.getPostConditions()) {
				for (Event e2 : c.getPostEvents()) {
					if (e2.isDisabledByAntiOclet())
						continue;
					if (e2.isDisabledByConflict())
						continue;

					postEvents.add(e2);
				}
			}
			
			eventArcs.put(e, postEvents);
		}
		
		for (Entry<Event, Set<Event>> arcs : eventArcs.entrySet()) {
			
			for (Event post : arcs.getValue()) {
			
				eventID++;
				b.append("    <lpoArc id=\"arc"+eventID+"\" ");
						b.append("provider=\"lpo.provider.edge\" ");
						b.append("source=\"event"+eventIDs.get(arcs.getKey())+"\" ");
						b.append("target=\"event"+eventIDs.get(post)+"\">\n");
				b.append("      <graphics userDrawn=\"true\">\n");
				b.append("        <line color=\"rgb(0, 0, 0)\" shape=\"line\" style=\"solid\" width=\"1\"/>\n");
				b.append("      </graphics>\n");
				b.append("    </lpoArc>\n");
			}
		}
		
		b.append("  </lpo>");
		b.append("</pnml>");
		

		ActionHelper.writeFile(path, b.toString());
	}
}

