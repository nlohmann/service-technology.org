package hub.top.editor.lola.text.syntax;

/**
 * DO NOT EDIT - Syntax generated from LolaIdent.syntax
 * at Wed Apr 08 17:04:21 CEST 2009
 * by hub.sam.tef.rcc.util.SourceGenerator.
 */

import hub.sam.tef.rcc.syntax.*;

public final class LolaIdentSyntax extends Syntax
{
	/**
	 * @generated
	 */
	private static final long serialVersionUID = -1239203061171L;

	public static final String Ascii = "Ascii";
	public static final String DisallowedLola = "DisallowedLola";
	public static final String lolaident = "lolaident";
	public static final String token = "token";

	public LolaIdentSyntax()	{
		super(15);

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
		rule.addRightSymbol("' '");
		addRule(rule);

		rule = new Rule(DisallowedLola, 1);	// 9
		rule.addRightSymbol("`whitespace`");
		addRule(rule);

		rule = new Rule(lolaident, 1);	// 10
		rule.addRightSymbol("_Ascii_minus_DisallowedLola_LIST");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola_LIST", 2);	// 11
		rule.addRightSymbol("_Ascii_minus_DisallowedLola_LIST");
		rule.addRightSymbol("_Ascii_minus_DisallowedLola");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola_LIST", 1);	// 12
		rule.addRightSymbol("_Ascii_minus_DisallowedLola");
		addRule(rule);

		rule = new Rule("_Ascii_minus_DisallowedLola", 3);	// 13
		rule.addRightSymbol(Ascii);
		rule.addRightSymbol("-");
		rule.addRightSymbol(DisallowedLola);
		addRule(rule);

		rule = new Rule(token, 1);	// 14
		rule.addRightSymbol(lolaident);
		addRule(rule);
	}
}
