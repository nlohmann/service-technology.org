package org.st.anica.ui.preferences;

import org.eclipse.jface.preference.*;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.st.anica.ui.Activator;

/**
 * A preference page for the nitpicker GUI
 */

public class AnicaUI_PreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {

	public AnicaUI_PreferencePage() {
		super(GRID);
		setPreferenceStore(Activator.getDefault().getPreferenceStore());
		setDescription("Configuration for Anica");
	}
	
	/**
	 * Create the fields of the preference page by setting corresponding
	 * editors.
	 */
	@Override
	public void createFieldEditors() {
		addField(new StringFieldEditor(PreferenceConstants.P_ANICA_SERVER_URL, "Anica Server URL", getFieldEditorParent()));
    addField(new IntegerFieldEditor(PreferenceConstants.P_ANICA_SERVER_PORT, "Server Port", getFieldEditorParent()));
    addField(new IntegerFieldEditor(PreferenceConstants.P_ANICA_LOCAL_PORT, "Local Port", getFieldEditorParent()));
    addField(new IntegerFieldEditor(PreferenceConstants.P_ANICA_PACKETSIZE, "Maximum Packet Size (KB)", getFieldEditorParent()));
    addField(new IntegerFieldEditor(PreferenceConstants.P_ANICA_TIMEOUT, "Timeout for Reply from Anica (ms)", getFieldEditorParent()));
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {
	}
	
}