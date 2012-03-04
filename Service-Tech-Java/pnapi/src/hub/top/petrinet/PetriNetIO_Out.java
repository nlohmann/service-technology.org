/*****************************************************************************\
 * Copyright (c) 2010. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Petri Net API/Java
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.petrinet;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;

/**
 * 
 * Provides front-end methods for reading and writing Petri nets.
 * 
 * @author Dirk Fahland
 */
public class PetriNetIO_Out {
  
  public static final int FORMAT_DOT = 1;
  public static final int FORMAT_LOLA = 2;
  public static final int FORMAT_OWFN = 3;
  public static final int FORMAT_WOFLAN = 4;
  
  public static String toLoLA(String ident) {
    String result = ident.replace(' ', '_');
    result = result.replace('(', '_');
    result = result.replace(')', '_');
    result = result.replace('[', '_');
    result = result.replace(']', '_');
    result = result.replace('=', '_');
    result = result.replace(':', '_');
    result = result.replace('{', '_');
    result = result.replace('}', '_');
    result = result.replace(',', '_');
    result = result.replace(';', '_');

    return result;
  }
  
  public static String toLoLA(PetriNet net) {
    StringBuilder b = new StringBuilder();
    
    boolean labeled_net = !net.isUnlabeled();
    
    b.append("{\n");
    b.append("  input file:\n");
    b.append("  invocation:\n");
    b.append("  net size:     "+net.getInfo(false)+"\n");
    b.append("}\n\n");
    
    // ---------------------- places ------------------------
    b.append("PLACE");
      b.append("    ");
      Iterator<Place> place = net.getPlaces().iterator();
      while (place.hasNext()) {
        Place p = place.next();
        String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
        b.append(" "+toLoLA(placeName));
        if (place.hasNext()) b.append(",");
      }
      b.append(";\n");
    b.append("\n");
    
    // ---------------------- markings ------------------------
    b.append("MARKING\n");
    b.append("  ");
    boolean firstPlace = true;
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        if (!firstPlace) b.append(",");
        String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
        b.append(" "+toLoLA(placeName)+":"+p.getTokens());
        firstPlace = false;
      }
    }
    b.append(";\n\n");
    
    // ---------------------- transitions ------------------------
    for (Transition t : net.getTransitions()) {
      String transitionName = (labeled_net ? t.getUniqueIdentifier() : t.getName());
      b.append("TRANSITION "+toLoLA(transitionName)+"\n");
        // pre-places of the transition
        b.append("  CONSUME");
        place = t.getPreSet().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName)+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
        // post-places of the transition
        b.append("  PRODUCE");
        place = t.getPostSet().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName)+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
      b.append("\n");
    }
    return b.toString();
  }
  
  public static String toOWFN(PetriNet net) {
    StringBuilder b = new StringBuilder();
    
    boolean labeled_net = !net.isUnlabeled();
    
    b.append("{\n");
    b.append("  input file:\n");
    b.append("  invocation:\n");
    b.append("  net size:     "+net.getInfo(false)+"\n");
    b.append("}\n\n");
    
    // ---------------------- places ------------------------
    b.append("PLACE");
      // print roles
      if (net.getRoles().size() > 0) {
        b.append("  ROLES");
        Iterator<String> role = net.getRoles().iterator();
        while (role.hasNext()) {
          b.append(" "+toLoLA(role.next()));
          if (role.hasNext()) b.append(",");
        }
        b.append(";\n");
      }
      // print internal places
      b.append("  INTERNAL\n");
        b.append("    ");
        Iterator<Place> place = net.getPlaces().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName));
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");

      // print input places
      b.append("  INPUT\n");
        b.append("    "); /*
        place = net.getPlaces().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName));
          if (place.hasNext()) b.append(",");
        } */
        b.append(";\n");

      // print input places
      b.append("  OUTPUT\n");
        b.append("    "); /*
        place = net.getPlaces().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName));
          if (place.hasNext()) b.append(",");
        } */
        b.append(";\n");
    b.append("\n");
    
    // ---------------------- markings ------------------------
    b.append("INITIALMARKING\n");
    b.append("  ");
    boolean firstPlace = true;
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        if (!firstPlace) b.append(",");
        String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
        b.append(" "+toLoLA(placeName)+":"+p.getTokens());
        firstPlace = false;
      }
    }
    b.append(";\n\n");
    
    b.append("FINALCONDITION\n");
    b.append("  TRUE;\n");
    b.append("\n");
    
    // ---------------------- transitions ------------------------
    for (Transition t : net.getTransitions()) {
      String transitionName = (labeled_net ? t.getUniqueIdentifier() : t.getName());
      b.append("TRANSITION "+toLoLA(transitionName)+"\n");
        // print roles of the transition
        if (t.getRoles().size() > 0) {
          b.append("  ROLES");
          Iterator<String> role = t.getRoles().iterator();
          while (role.hasNext()) {
            b.append(" "+toLoLA(role.next()));
            if (role.hasNext()) b.append(",");
          }
          b.append(";\n");
        }
        // pre-places of the transition
        b.append("  CONSUME");
        place = t.getPreSet().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName)+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
        // post-places of the transition
        b.append("  PRODUCE");
        place = t.getPostSet().iterator();
        while (place.hasNext()) {
          Place p = place.next();
          String placeName = (labeled_net ? p.getUniqueIdentifier() : p.getName());
          b.append(" "+toLoLA(placeName)+":1");
          if (place.hasNext()) b.append(",");
        }
        b.append(";\n");
      b.append("\n");
    }
    
    return b.toString();
  }
  
  /**
   * @param format
   * @return standard file extension for given file format
   */
  public static String getFileExtension(int format) {
    switch(format) {
    case FORMAT_DOT: return "dot";
    case FORMAT_LOLA: return "lola";
    case FORMAT_OWFN: return "owfn";
    case FORMAT_WOFLAN: return "tpn";
    }
    return "unknown";
  }
  
  /**
   * @param format
   * @return standard file extension for given file format
   */
  public static int getFormatForFileExtension(String ext) {
    if ("dot".equals(ext)) return FORMAT_DOT;
    else if ("lola".equals(ext)) return FORMAT_LOLA;
    else if ("owfn".equals(ext)) return FORMAT_OWFN;
    else if ("tpn".equals(ext)) return FORMAT_WOFLAN;
    else return 0;
  }
  

  
  public static final int PARAM_SWIMLANE = 1;
  
  public static void writeToFile(PetriNet net, String fileName, int format, int parameter) throws IOException {

    String extendedFileName;
    int extIndex = fileName.lastIndexOf('.');
    if (extIndex >= 0) {
      String ext = fileName.substring(extIndex+1);
      if (!ext.equals(getFileExtension(format)))
        extendedFileName = fileName+"."+getFileExtension(format);
      else
        extendedFileName = fileName;
    } else {
      extendedFileName = fileName+"."+getFileExtension(format);
    }
    
    // Create file 
    FileWriter fstream = new FileWriter(extendedFileName);
    BufferedWriter out = new BufferedWriter(fstream);
    
    if (format == FORMAT_DOT) {
      if ((parameter & PARAM_SWIMLANE) == 1)
        out.write(net.toDot_swimlanes());
      else
        out.write(net.toDot());
    } else if (format == FORMAT_LOLA) {
      out.write(PetriNetIO_Out.toLoLA(net));
    } else if (format == FORMAT_OWFN) {
      out.write(PetriNetIO_Out.toOWFN(net));
    }

    //Close the output stream
    out.close();
  }
  
}
