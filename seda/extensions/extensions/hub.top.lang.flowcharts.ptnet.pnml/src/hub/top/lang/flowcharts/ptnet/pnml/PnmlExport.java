/**
*
*/

package hub.top.lang.flowcharts.ptnet.pnml;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

import com.thaiopensource.validate.ValidationDriver;

import fr.lip6.move.pnml.framework.general.AbstractPnmlImportExport;
import fr.lip6.move.pnml.framework.general.ImportCustomPNMLFileType;
import fr.lip6.move.pnml.framework.general.PNMLFileRepository;
import fr.lip6.move.pnml.framework.general.PNMLFileType;
import fr.lip6.move.pnml.framework.general.OfficialPNMLFileType;
import fr.lip6.move.pnml.framework.hlapi.HLAPIClass;
import fr.lip6.move.pnml.framework.utils.ModelRepository;
import fr.lip6.move.pnml.framework.utils.exception.BadFileFormatException;
import fr.lip6.move.pnml.framework.utils.exception.OCLValidationFailed;
import fr.lip6.move.pnml.framework.utils.exception.OtherException;
import fr.lip6.move.pnml.framework.utils.exception.UnhandledNetType;
import fr.lip6.move.pnml.framework.utils.exception.ValidationFailedException;

/**
* This class is used for exporting a PNMLDocument object to a PNML file.
*
* @author Guillaume Giffo
*
*/
public abstract class PnmlExport extends AbstractPnmlImportExport {

   /**
    * the file name.
    */
   private File out;

   /**
    *
    */
   public PnmlExport() {
       super("export", null);
   }

   /**
    *
    * @param filepath
    *            the path to the custom plug in registration file
    */
   public PnmlExport(String filepath) {
       super("export", filepath);
   }

   /**
    * Creates a PNML file from a PetrynetDocHLAPI object. The prettyPrinting is
    * used accordingly to the modelRepository pretty printing status.
    *
    * @param object
    *            the object to export.
    * @param filepath
    *            the file path.
    * @throws UnhandledNetType
    *             if the net type is unknown.
    * @throws OCLValidationFailed
    *             if an ocl violation is detected.
    * @throws IOException
    *             if a problem occur with file.
    * @throws ValidationFailedException
    *             if the validation fail.
    * @throws OtherException .
    * @throws BadFileFormatException .
    */
   public final void exportObject(HLAPIClass object, String filepath)
           throws UnhandledNetType, OCLValidationFailed, IOException,
           ValidationFailedException, BadFileFormatException, OtherException {

       out = new File(filepath);

       final String classname = object.getClass().getName();

       PNMLFileType otype = OfficialPNMLFileType.getByClassName(classname);

       // If the type is not known looking for it in custom types file.
       if (otype == null && newTypeFilePath != null) {
           final PNMLFileRepository impo = new ImportCustomPNMLFileType(
                   newTypeFilePath);
           otype = impo.getByClassName(classname);
       }

       // If not find in custom type...
       if (otype == null) {
           log
                   .error("type "
                           + object.getClass().getName()
                           + " is unknown, we are expecting a HLAPIRootClass (like PetriNetDocHLAPI) object of a known package.");
           throw new UnhandledNetType("type " + object.getClass().getName()
                   + " is unknown");
       }

       doWork(object, otype.getRngUrl());

   }

   /**
    * Creates a PNML file from a PetrynetDocHLAPI object. The prettyPrinting is
    * used accordingly to the modelRepository pretty printing status.
    *
    * @param object
    *            the object to export.
    * @param filepath
    *            the file path.
    * @param usePrettyPrint
    *            true if you want to use pretty print, false either.
    * @throws UnhandledNetType
    *             if the net type is unknown.
    * @throws OCLValidationFailed
    *             if an ocl violation is detected.
    * @throws IOException
    *             if a problem occur with file.
    * @throws ValidationFailedException
    *             if the validation fail.
    * @throws OtherException .
    * @throws BadFileFormatException .
    */
   public final void exportObject(HLAPIClass object, String filepath,
           boolean usePrettyPrint) throws UnhandledNetType,
           OCLValidationFailed, IOException, ValidationFailedException,
           BadFileFormatException, OtherException {
       ModelRepository.getInstance().setPrettyPrintStatus(usePrettyPrint);
       exportObject(object, filepath);
   }

   /**
    * This class checks OCL violation, exports the file, and validates it.
    *
    * @param pndoc
    *            the pnml document object.
    * @param schemafile
    *            the schemat file url.
    * @throws OCLValidationFailed
    *             if an ocl violation is detected.
    * @throws IOException
    *             if a problem occur with file access.
    * @throws ValidationFailedException
    *             if the pnml validation fail.
    */
   private void doWork(HLAPIClass pndoc, String schemafile)
           throws OCLValidationFailed, IOException, ValidationFailedException {

       oclChecking(pndoc);
       log.trace("OCL ok, writting temporary file");

       String outputPre = pndoc.toPNML();
       final String output = postProcess(outputPre);
       final Writer writefile = new BufferedWriter(new FileWriter(out));
       writefile.write(output);
       writefile.close();

       log.trace("writing ok");
       log.trace("validation");

       rngGrammarValidation(schemafile, ValidationDriver
               .uriOrFileInputSource(out.getAbsolutePath()));
   }
   
   public abstract String postProcess(String output);
}
