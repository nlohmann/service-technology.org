package hub.top.mia;

import java.util.Comparator;

public class LexicMarkingComparator implements Comparator<SafeMarking>{

	@Override
	public int compare(SafeMarking o1, SafeMarking o2) {
		String s1_Old, s1_New, s2_Old, s2_New;
		
		s1_Old = o1.getM_Old().toString();
		s1_New = o1.getM_New().toString();
		
		s2_Old = o2.getM_Old().toString();
		s2_New = o2.getM_New().toString();
		
		if (s1_Old.compareTo(s2_Old) < 0 && s1_New.compareTo(s2_New) < 0) {
			return 1;
		}
		
		return 0;
	}

}
