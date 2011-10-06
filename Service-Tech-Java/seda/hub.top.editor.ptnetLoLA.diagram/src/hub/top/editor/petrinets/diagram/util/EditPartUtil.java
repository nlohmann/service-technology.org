package hub.top.editor.petrinets.diagram.util;

public class EditPartUtil {
	
	private static int LINE_LENGTH_LABEL = 30;

	public static String insertLineBreaks(String text, int maxLine) {
		// format string to a suitable length
		int i = 0;
		String newText = "";
		while (i < text.length()) {
			newText += text.charAt(i++);
			if (i % maxLine == 0) {
				newText += '\n';
			}
		}
		return newText;
	}
	
	public static String insertLineBreaks(String text) {
		return insertLineBreaks(text, LINE_LENGTH_LABEL);
	}
}
