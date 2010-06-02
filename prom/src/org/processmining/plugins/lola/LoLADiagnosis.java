package org.processmining.plugins.lola;

import java.util.HashMap;

import org.processmining.framework.plugin.PluginContext;
import org.processmining.framework.util.HTMLToString;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.models.semantics.petrinet.CTMarking;
import org.processmining.plugins.configuration.Configuration;
import org.processmining.plugins.configuration.ConfigurationGroup;
import org.processmining.plugins.configuration.ConfigurationList;

/**
 * Visualizer for the results of LoLA.
 * 
 * @author nlohmann
 * @email niels.lohmann@uni-rostock.de
 * @since March 2010
 * @version March 2010
 */
public class LoLADiagnosis implements HTMLToString {
	private Petrinet net;
	private Configuration cfg;
	
	public LoLADiagnosis(PluginContext context, Petrinet net, Configuration cfg) {
		this.net = net;
		this.cfg = cfg;
	}

	/**
	 * Adds HTML tags to the given text indicating that the given text relates
	 * to an error.
	 * 
	 * @param text
	 *            The given text.
	 * @return The given text with HTML tags added.
	 */
	private String error(String text) {
		return "<font color=\"FF0000\"><b>" + text + "</b></font>";
	}

	/**
	 * Adds HTML tags to the given text indicating that the given text relates
	 * to a warning.
	 * 
	 * @param text
	 *            The given text.
	 * @return The given text with HTML tags added.
	 */
	private String warning(String text) {
		return "<font color=\"FF8800\"><b>" + text + "</b></font>";
	}

	/**
	 * Adds HTML tags to the given text indicating that the given text relates
	 * to something good.
	 * 
	 * @param text
	 *            The given text.
	 * @return The given text with HTML tags added.
	 */
	private String okay(String text) {
		return "<font color=\"008800\"><b>" + text + "</b></font>";
	}

	/**
	 * Adds HTML tags to the given text indicating that the given text relates
	 * to something yet undecided.
	 * 
	 * @param text
	 *            The given text.
	 * @return The given text with HTML tags added.
	 */
	@SuppressWarnings("unused")
	private String strong(String text) {
		return "<b>" + text + "</b>";
	}


	public String toHTMLString(boolean includeHTMLTags) {
		// descriptions of the tasks and reduction techniques
		HashMap<String, String> lolaNames = new HashMap<String, String>();
		lolaNames.put("BOUNDEDNET",    "check if the net is bounded");
		lolaNames.put("DEADLOCK",      "check if a dead marking is reachable");
		lolaNames.put("HOME",          "check if the net has home markings");
		lolaNames.put("REVERSIBILITY", "check if the net is reversible");
		
		lolaNames.put("STUBBORN",   "partial order reduction using stubborn sets (<a href=\"http://www.springerlink.com/content/mtj3p6183xchedr9/\">more</a>)");
		lolaNames.put("COVER",      "infinite sequence abstraction using the coverability graph technique (<a href=\"http://www.springerlink.com/content/h75484731mh5826u/\">more</a>)");
		lolaNames.put("SMALLSTATE", "use light weight data structure for markings");
		lolaNames.put("PREDUCTION", "compress markings using P-invariants (<a href=\"http://www.springerlink.com/content/20v8eyakvde5e558/\">more</a>)");

		String[] reductionTechniques = {"STUBBORN", "COVER", "PREDUCTION", "SMALLSTATE"};

		// header
		StringBuffer buffer = new StringBuffer();
		if (includeHTMLTags) {
			buffer.append("<html>");
		}
		buffer.append("<head><style type=\"text/css\">h1, h2, h3, li, ul, p { font-family: sans-serif; } </style>");
		buffer.append("<title>LoLA results for net \"" + net.getLabel() + "\"</title>");
		buffer.append("</head><body>");
		buffer.append("<h1>LoLA Results for Net \"" + net.getLabel() + "\"</h1>");

		// boundedness
		Boolean unbounded = cfg.getValue("unbounded.result", null);
		if (unbounded != null) {
			buffer.append("<h2>Boundedness Result</h2>");
			buffer.append("<ul>");
			if (unbounded) {
				buffer.append("<li>The net is " + error("unbounded") + ".");
				buffer.append("<ul>");
				ConfigurationList state = cfg.getValue("unbounded.state", null);
				CTMarking m = LoLA_Executor.constructMarking(net, state);
				buffer.append("<li>unbounded place(s): " + warning(m.getOmegaPlaces().toString()) + "</li>");
				buffer.append("<li>a witness marking: " + warning(m.toHTMLString(false)) + "</li>");
				ConfigurationList path = cfg.getValue("unbounded.path", null);
				if (path != null) {
					buffer.append("<li>path expression to generate this marking: ");
					for (int i = 0; i < path.size(); ++i) {
						if (i > 0) {
							buffer.append(", ");
						}
						buffer.append(warning(((Transition)LoLA_Executor.getNode(path.getValue(i+ ".0", ""))).getLabel()));
					}
				}
				ConfigurationList cycle = cfg.getValue("unbounded.cycle", null);
				if (cycle != null) {
					buffer.append(" (");
					for (int i = 0; i < cycle.size(); ++i) {
						if (i > 0) {
							buffer.append(", ");
						}
						buffer.append(warning(((Transition)LoLA_Executor.getNode(cycle.getValue(i+ ".0", ""))).getLabel()));
					}
					buffer.append(")*</li>");
				}
				buffer.append("</ul></li>");
			} else {
				buffer.append("<li>The net is " + okay("bounded") + ".</li>");
			}
			buffer.append("</ul>");
		}

		// deadlock freedom
		Boolean deadlock = cfg.getValue("deadlock.result", null);
		if (deadlock != null) {
			buffer.append("<h2>Deadlock Freedom Result</h2>");
			buffer.append("<ul>");
			if (deadlock) {
				buffer.append("<li>The net " + error("contains a deadlock") + ".");
				buffer.append("<ul>");
				ConfigurationList state = cfg.getValue("deadlock.state", null);
				CTMarking m = LoLA_Executor.constructMarking(net, state);
				buffer.append("<li>deadlock state: " + warning(m.toHTMLString(false)) + "</li>");
				ConfigurationList path = cfg.getValue("deadlock.path", null);
				if (path != null) {
					buffer.append("<li>path to generate this marking: ");
					for (int i = 0; i < path.size(); ++i) {
						if (i > 0) {
							buffer.append(", ");
						}
						buffer.append(warning(((Transition)LoLA_Executor.getNode(path.getValue(i+ ".0", ""))).getLabel()));
					}
				}
				buffer.append("</ul></li>");

			} else {
				buffer.append("<li>The net is " + okay("deadlock free") + ".</li>");
			}
			buffer.append("</ul>");
		}

		// reversibility
		Boolean reversibility = cfg.getValue("reversibility.result", null);
		if (reversibility != null) {
			buffer.append("<h2>Reversibility Result</h2>");
			buffer.append("<ul>");
			if (reversibility) {
				buffer.append("<li>The net is " + okay("reversible") + ".</li>");				
			} else {
				buffer.append("<li>The net is " + error("not reversible") + ".</li>");				
				ConfigurationList state = cfg.getValue("reversibility.state", null);
				CTMarking m = LoLA_Executor.constructMarking(net, state);
				buffer.append("<li>the initial marking is not reachable from " + warning(m.toHTMLString(false)) + "</li>");
			}
			buffer.append("</ul>");
		}
		
		// home markings
		Boolean home = cfg.getValue("homemarking.result", null);
		if (home != null) {
			buffer.append("<h2>Home Marking Result</h2>");
			buffer.append("<ul>");
			if (home) {
				buffer.append("<li>The net " + okay("has home markings") + ".</li>");				
				ConfigurationList state = cfg.getValue("homemarking.state", null);
				CTMarking m = LoLA_Executor.constructMarking(net, state);
				buffer.append("<li>a home marking: " + warning(m.toHTMLString(false)) + "</li>");
			} else {
				buffer.append("<li>The net " + error("has no home markings") + ".</li>");				
			}
			buffer.append("</ul>");
		}
		
		// configuration
		buffer.append("<h2>LoLA's Configuration</h2>");
		buffer.append("<ul>");
		String t = cfg.getValue("configuration.verification", "");
		buffer.append("<li>verification task: " + lolaNames.get(t) +"</li>");
		ConfigurationGroup reduction = cfg.getValue("configuration.reduction", null);
		if (reduction != null) {
			buffer.append("<li>reduction techniques used:</li><ul>");
			for (String s : reductionTechniques) {
				if (cfg.getValue("configuration.reduction." + s, false)) {
					buffer.append("<li>" + lolaNames.get(s) + "</li>");
				}
			}
			buffer.append("</ul></li>");
		} else {
			buffer.append("<li>no reductions used</li>");
		}
		buffer.append("</ul>");
	
		// statistics
		buffer.append("<h2>Statistics</h2>");
		buffer.append("<ul>");
		buffer.append("<li>stored markings: " + cfg.getValue("statistics.states", 0));
		buffer.append("<li>fired transitions: " + cfg.getValue("statistics.edges", 0));
		buffer.append("</ul>");
		
		buffer.append("</body>");
		if (includeHTMLTags) {
			buffer.append("</html>");
		}
		return buffer.toString();
	}
}
