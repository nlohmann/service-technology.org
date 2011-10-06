package hub.top.editor.ptnetLoLA.diagram.edit.parts;

import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ITextAwareEditPart;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrapLabel;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;

/**
 * @generated
 */
public class PtnetLoLAEditPartFactory implements EditPartFactory {

	/**
	 * @generated
	 */
	public EditPart createEditPart(EditPart context, Object model) {
		if (model instanceof View) {
			View view = (View) model;
			switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(view)) {

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart(
						view);

			case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID:
				return new hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart(
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
		if (source.getFigure() instanceof WrappingLabel)
			return new TextCellEditorLocator((WrappingLabel) source.getFigure());
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
		private WrappingLabel wrapLabel;

		/**
		 * @generated
		 */
		public TextCellEditorLocator(WrappingLabel wrapLabel) {
			this.wrapLabel = wrapLabel;
		}

		/**
		 * @generated
		 */
		public WrappingLabel getWrapLabel() {
			return wrapLabel;
		}

		/**
		 * @generated
		 */
		public void relocate(CellEditor celleditor) {
			Text text = (Text) celleditor.getControl();
			Rectangle rect = getWrapLabel().getTextBounds().getCopy();
			getWrapLabel().translateToAbsolute(rect);
			if (getWrapLabel().isTextWrapOn()
					&& getWrapLabel().getText().length() > 0) {
				rect.setSize(new Dimension(text.computeSize(rect.width, SWT.DEFAULT)));
			} else {
				int avr = FigureUtilities.getFontMetrics(text.getFont())
						.getAverageCharWidth();
				rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT, SWT.DEFAULT))
						.expand(avr * 2, 0));
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
			rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT, SWT.DEFAULT))
					.expand(avr * 2, 0));
			if (!rect.equals(new Rectangle(text.getBounds()))) {
				text.setBounds(rect.x, rect.y, rect.width, rect.height);
			}
		}
	}
}
