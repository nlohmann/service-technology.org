package hub.top.adaptiveSystem.diagram.edit.parts;

import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ITextAwareEditPart;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrapLabel;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;

/**
 * @generated
 */
public class AdaptiveSystemEditPartFactory implements EditPartFactory {

	/**
	 * @generated
	 */
	public EditPart createEditPart(EditPart context, Object model) {
		if (model instanceof View) {
			View view = (View) model;
			switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(view)) {

			case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart(
						view);

			case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
				return new hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart(
						view);
			}
		}
		return createUnrecognizedEditPart(context, model);
	}

	/**
	 * @generated
	 */
	private EditPart createUnrecognizedEditPart(EditPart context, Object model) {
		// Handle creation of unrecognized child node EditParts here
		return null;
	}

	/**
	 * @generated
	 */
	public static CellEditorLocator getTextCellEditorLocator(
			ITextAwareEditPart source) {
		if (source.getFigure() instanceof WrapLabel)
			return new TextCellEditorLocator((WrapLabel) source.getFigure());
		else {
			return new LabelCellEditorLocator((Label) source.getFigure());
		}
	}

	/**
	 * @generated
	 */
	static private class TextCellEditorLocator implements CellEditorLocator {

		/**
		 * @generated
		 */
		private WrapLabel wrapLabel;

		/**
		 * @generated
		 */
		public TextCellEditorLocator(WrapLabel wrapLabel) {
			this.wrapLabel = wrapLabel;
		}

		/**
		 * @generated
		 */
		public WrapLabel getWrapLabel() {
			return wrapLabel;
		}

		/**
		 * @generated
		 */
		public void relocate(CellEditor celleditor) {
			Text text = (Text) celleditor.getControl();
			Rectangle rect = getWrapLabel().getTextBounds().getCopy();
			getWrapLabel().translateToAbsolute(rect);
			if (getWrapLabel().isTextWrapped()
					&& getWrapLabel().getText().length() > 0) {
				rect.setSize(new Dimension(text.computeSize(rect.width,
						SWT.DEFAULT)));
			} else {
				int avr = FigureUtilities.getFontMetrics(text.getFont())
						.getAverageCharWidth();
				rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT,
						SWT.DEFAULT)).expand(avr * 2, 0));
			}
			if (!rect.equals(new Rectangle(text.getBounds()))) {
				text.setBounds(rect.x, rect.y, rect.width, rect.height);
			}
		}

	}

	/**
	 * @generated
	 */
	private static class LabelCellEditorLocator implements CellEditorLocator {

		/**
		 * @generated
		 */
		private Label label;

		/**
		 * @generated
		 */
		public LabelCellEditorLocator(Label label) {
			this.label = label;
		}

		/**
		 * @generated
		 */
		public Label getLabel() {
			return label;
		}

		/**
		 * @generated
		 */
		public void relocate(CellEditor celleditor) {
			Text text = (Text) celleditor.getControl();
			Rectangle rect = getLabel().getTextBounds().getCopy();
			getLabel().translateToAbsolute(rect);
			int avr = FigureUtilities.getFontMetrics(text.getFont())
					.getAverageCharWidth();
			rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT,
					SWT.DEFAULT)).expand(avr * 2, 0));
			if (!rect.equals(new Rectangle(text.getBounds()))) {
				text.setBounds(rect.x, rect.y, rect.width, rect.height);
			}
		}
	}
}
