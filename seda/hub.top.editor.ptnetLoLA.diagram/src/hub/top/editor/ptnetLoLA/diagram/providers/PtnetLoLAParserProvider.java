package hub.top.editor.ptnetLoLA.diagram.providers;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.service.AbstractProvider;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.GetParserOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParserProvider;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.ui.services.parser.ParserHintAdapter;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class PtnetLoLAParserProvider extends AbstractProvider implements
		IParserProvider {

	/**
	 * @generated
	 */
	private IParser transitionName_4001Parser;

	/**
	 * @generated
	 */
	private IParser getTransitionName_4001Parser() {
		if (transitionName_4001Parser == null) {
			transitionName_4001Parser = createTransitionName_4001Parser();
		}
		return transitionName_4001Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createTransitionName_4001Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser placeName_4002Parser;

	/**
	 * @generated
	 */
	private IParser getPlaceName_4002Parser() {
		if (placeName_4002Parser == null) {
			placeName_4002Parser = createPlaceName_4002Parser();
		}
		return placeName_4002Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createPlaceName_4002Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser placeToken_4003Parser;

	/**
	 * @generated
	 */
	private IParser getPlaceToken_4003Parser() {
		if (placeToken_4003Parser == null) {
			placeToken_4003Parser = createPlaceToken_4003Parser();
		}
		return placeToken_4003Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createPlaceToken_4003Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getPlace_Token(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToPlaceWeight_4004Parser;

	/**
	 * @generated
	 */
	private IParser getArcToPlaceWeight_4004Parser() {
		if (arcToPlaceWeight_4004Parser == null) {
			arcToPlaceWeight_4004Parser = createArcToPlaceWeight_4004Parser();
		}
		return arcToPlaceWeight_4004Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToPlaceWeight_4004Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToTransitionWeight_4005Parser;

	/**
	 * @generated
	 */
	private IParser getArcToTransitionWeight_4005Parser() {
		if (arcToTransitionWeight_4005Parser == null) {
			arcToTransitionWeight_4005Parser = createArcToTransitionWeight_4005Parser();
		}
		return arcToTransitionWeight_4005Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToTransitionWeight_4005Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	protected IParser getParser(int visualID) {
		switch (visualID) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID:
			return getTransitionName_4001Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID:
			return getPlaceName_4002Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart.VISUAL_ID:
			return getPlaceToken_4003Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID:
			return getArcToPlaceWeight_4004Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID:
			return getArcToTransitionWeight_4005Parser();
		}
		return null;
	}

	/**
	 * @generated
	 */
	public IParser getParser(IAdaptable hint) {
		String vid = (String) hint.getAdapter(String.class);
		if (vid != null) {
			return getParser(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(vid));
		}
		View view = (View) hint.getAdapter(View.class);
		if (view != null) {
			return getParser(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(view));
		}
		return null;
	}

	/**
	 * @generated
	 */
	public boolean provides(IOperation operation) {
		if (operation instanceof GetParserOperation) {
			IAdaptable hint = ((GetParserOperation) operation).getHint();
			if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
					.getElement(hint) == null) {
				return false;
			}
			return getParser(hint) != null;
		}
		return false;
	}

	/**
	 * @generated
	 */
	public static class HintAdapter extends ParserHintAdapter {

		/**
		 * @generated
		 */
		private final IElementType elementType;

		/**
		 * @generated
		 */
		public HintAdapter(IElementType type, EObject object, String parserHint) {
			super(object, parserHint);
			assert type != null;
			elementType = type;
		}

		/**
		 * @generated
		 */
		public Object getAdapter(Class adapter) {
			if (IElementType.class.equals(adapter)) {
				return elementType;
			}
			return super.getAdapter(adapter);
		}
	}

}
