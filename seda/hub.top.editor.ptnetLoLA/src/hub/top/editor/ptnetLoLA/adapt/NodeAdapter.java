package hub.top.editor.ptnetLoLA.adapt;

import org.eclipse.emf.common.notify.Notification;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;

public class NodeAdapter extends org.eclipse.emf.common.notify.impl.AdapterImpl {

	public NodeAdapter() {
		super();
		System.err.println("creating NodeAdapter");
	}
	
	@Override
	public boolean isAdapterForType(Object type) {
		if (type instanceof Node)
			return true;
		return false;
	}
	
	@Override
	public void notifyChanged(Notification msg) {
		System.err.println("notifyChanged: "+msg);
	}
}
