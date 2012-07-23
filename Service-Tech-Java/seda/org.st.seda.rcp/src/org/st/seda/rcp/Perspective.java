package org.st.seda.rcp;

import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;

public class Perspective implements IPerspectiveFactory {

	@Override
	public void createInitialLayout(IPageLayout layout) {
		String editorArea = layout.getEditorArea();
		
		layout.setEditorAreaVisible(true);
		layout.setFixed(false);
		
		layout.addStandaloneView("org.eclipse.ui.navigator.ProjectExplorer",  true /* show title */, IPageLayout.LEFT, 0.25f, editorArea);
		layout.addView(IPageLayout.ID_PROP_SHEET, IPageLayout.BOTTOM, 0.75f, editorArea);
	}

}
