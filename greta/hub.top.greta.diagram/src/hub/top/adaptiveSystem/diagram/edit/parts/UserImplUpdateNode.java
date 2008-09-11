package hub.top.adaptiveSystem.diagram.edit.parts;

import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.swt.graphics.Color;

public class UserImplUpdateNode {
	protected static Condition condition = null;
	protected static Event event = null;
	
	static final Color ABSTRACTNODE_BACK = new Color(null, 200, 200, 200);
	static final Color DISABLED_FRONT = new Color(null, 190, 190, 190);
	static final Color DISABLED_BACK = new Color(null, 230, 230, 230);
	
	/**
	 * Manja Wolf
	 * calls updateFaceCondition(Ellipse ellipse) if condition is from preNet 
	 */
	public static void updateCondition(ConditionPreNetEditPart element, IFigure ellipse) {
		//System.out.println("Start UserImplUpdateNode.updateCondition(ConditionPreNetEditPart).");
		condition = (Condition) ((Node) element.getModel()).getElement();
		updateFaceCondition((Ellipse) ellipse);
		
	}
	
	/**
	 * Manja Wolf
	 * calls updateFaceCondition(Ellipse ellipse) if condition is from doNet 
	 */
	public static void updateCondition(ConditionDoNetEditPart element, IFigure ellipse) {
		//System.out.println("Start UserImplUpdateNode.updateCondition(ConditionPreNetEditPart).");
		condition = (Condition) ((Node) element.getModel()).getElement();
		updateFaceCondition((Ellipse) ellipse);

	}
	
	/**
	 * Manja Wolf
	 * calls updateFaceCondition(Ellipse ellipse) if condition is from mainProcess 
	 */
	public static void updateCondition(ConditionAPEditPart element, IFigure ellipse) {
		//System.out.println("Start UserImplUpdateNode.updateCondition(ConditionMainProcessEditPart).");
		condition = (Condition) ((Node) element.getModel()).getElement();
		updateFaceCondition((Ellipse) ellipse);
		//System.out.println("Ende UserImplUpdateNode.updateConditionMainProcessEditPart.");
	}
	
	
	public static void updateFaceCondition(Ellipse figure) {
		//System.out.println("Start updateFaceCondition.");
		if (condition.isSetDisabledByAntiOclet() && condition.isDisabledByAntiOclet() 
				|| condition.isSetDisabledByConflict() && condition.isDisabledByConflict()) {
			figure.setLineStyle(Graphics.LINE_SOLID);
			figure.setLineWidth(1);
			figure.setForegroundColor(DISABLED_FRONT);
			figure.setBackgroundColor(DISABLED_BACK);
		} else {
			/**
			 * Condition färben je nach Attribute Abstract: true == dashed line
			 */
			if (condition.isSetAbstract() && condition.isAbstract()) {
				//System.out.println("MW: Die Condition ist abstract!! ");
				figure.setLineStyle(Graphics.LINE_DASH);
				figure.setLineWidth(1);
				figure.setForegroundColor(ColorConstants.gray);
				figure.setBackgroundColor(ABSTRACTNODE_BACK);
			} else {
				//System.out.println("MW: Die Condition ist NICHT abstract!! ");
	
				/**
				 * Condition färben je nach Attribute temp: 
				 * 0 == cold --> blau 
				 * 1 == without --> alles bleibt wie es ist 
				 * 2 == hot --> rot
				 */
				switch (condition.getTemp().getValue()) {
				case 2:
					//System.out.println("MW: Die Condition ist hot!! ");
					figure.setLineWidth(2);
					figure.setForegroundColor(ColorConstants.red);
					figure.setBackgroundColor(ColorConstants.white);
					break;
				case 1:
					//System.out.println("MW: Die Condition ist without und somit passiv ");
					figure.setLineWidth(1);
					figure.setForegroundColor(ColorConstants.black);
					figure.setBackgroundColor(ABSTRACTNODE_BACK);
					figure.setLineStyle(Graphics.LINE_SOLID);
					break;
				case 0:
					//System.out.println("MW: Die Condition ist cold!! ");
					figure.setLineWidth(2);
					figure.setForegroundColor(ColorConstants.blue);
					figure.setBackgroundColor(ColorConstants.white);
					break;
				}
			}
		}
			//System.out.println("END updateFaceCondition().");
		
	}
	
	
		/**
		 * Manja Wolf
		 * calls updateFaceEvent(RectangleFigure figure) if event is from preNet 
		 */
		public static void updateEvent(EventPreNetEditPart element, IFigure rectangle) {
			//System.out.println("Start UserImplUpdateNode.updateEvent(EventPreNetEditPart).");
			event = (Event) ((Node) element.getModel()).getElement();
			updateFaceEvent((RectangleFigure) rectangle);
		}
		
		/**
		 * Manja Wolf
		 * calls updateFaceEvent(RectangleFigure figure) if event is from doNet 
		 */
		public static void updateEvent(EventDoNetEditPart element, IFigure rectangle) {
			//System.out.println("Start UserImplUpdateNode.updateEvent(EventDoNetEditPart).");
			event = (Event) ((Node) element.getModel()).getElement();
			updateFaceEvent((RectangleFigure) rectangle);
		}
		
		/**
		 * Manja Wolf
		 * calls updateFaceEvent(RectangleFigure figure) if event is from mainProcess
		 */
		public static void updateEvent(EventAPEditPart element, IFigure rectangle) {
			//System.out.println("Start UserImplUpdateNode.updateEvent(EventMainProcessEditPart).");
			event = (Event) ((Node) element.getModel()).getElement();
			updateFaceEvent((RectangleFigure) rectangle);
		}
	
		
		/**
		 * Draw the Event depending on attributes
		 * 
		 * @param figure
		 */
		public static void updateFaceEvent(RectangleFigure figure) {
			//System.out.println("Start updateFaceEvent.");
			if (event.isSetDisabledByAntiOclet() && event.isDisabledByAntiOclet() 
					|| event.isSetDisabledByConflict() && event.isDisabledByConflict()) {
				figure.setLineStyle(Graphics.LINE_SOLID);
				figure.setLineWidth(1);
				figure.setForegroundColor(DISABLED_FRONT);
				figure.setBackgroundColor(DISABLED_BACK);
			} else {
				/**
				 * Event färben je nach Attribute Abstract: true == dashed line
				 */
				if (event.isSetAbstract() && event.isAbstract()) {
					//System.out.println("MW: Das Event ist abstract!! ");
					figure.setLineStyle(Graphics.LINE_DASH);
					figure.setLineWidth(1);
					figure.setForegroundColor(ColorConstants.gray);
					figure.setBackgroundColor(ABSTRACTNODE_BACK);
				} else {
					//System.out.println("MW: Das Event ist NICHT abstract!! ");
	
					/**
					 * Event färben je nach Attribute temp: 
					 * 0 == cold --> blau 
					 * 1 == without --> alles bleibt wie es ist 
					 * 2 == hot --> rot
					 */
					switch (event.getTemp().getValue()) {
					case 2:
						//System.out.println("MW: Das Event ist hot!! ");
						figure.setLineWidth(2);
						figure.setForegroundColor(ColorConstants.red);
						figure.setBackgroundColor(ColorConstants.white);
						break;
					case 1:
						//System.out.println("MW: Das Event ist without!! ");
						figure.setLineWidth(1);
						figure.setForegroundColor(ColorConstants.black);
						figure.setBackgroundColor(ABSTRACTNODE_BACK);
						figure.setLineStyle(Graphics.LINE_SOLID);
						break;
					case 0:
						//System.out.println("MW: Das Event ist cold!! ");
						figure.setLineWidth(2);
						figure.setForegroundColor(ColorConstants.blue);
						figure.setBackgroundColor(ColorConstants.white);
						break;
					}
				}
			}
		}
		
}
