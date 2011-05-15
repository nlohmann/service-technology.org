package org.st.scenarios.test;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.junit.Test;
import org.st.scenarios.clsc.CLSCLexer;
import org.st.scenarios.clsc.CLSCParser;
import org.st.scenarios.clsc.Specification;

import junit.framework.Assert;
import junit.framework.TestSuite;

public class ParserTest extends TestSuite {

	@Test
	public void parse_ems () throws Exception {
		
		String testFileRoot = System.getProperty("test.testFileRoot", TranslationTest.defaultTestDir);
		String sourceFile = testFileRoot+"/ems.clsc";
		
		CLSCLexer lex = new CLSCLexer(new ANTLRFileStream(sourceFile));
       	CommonTokenStream tokens = new CommonTokenStream(lex);

        CLSCParser parser = new CLSCParser(tokens);

        Specification result = parser.net();
        Assert.assertNotNull(result);
        
        System.out.println(result.toDot());
        
	}
}
