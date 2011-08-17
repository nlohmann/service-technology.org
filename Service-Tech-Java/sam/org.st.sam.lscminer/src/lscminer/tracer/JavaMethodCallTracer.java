package lscminer.tracer;

import gov.nasa.jpf.*;
import gov.nasa.jpf.jvm.*;
import gov.nasa.jpf.jvm.bytecode.*;
import java.util.*;
import java.io.*;
import lscminer.datastructure.LSCEvent;

/**
 * This class can trace method calls from an arbitrary Java program,
 * given its class path and a set of test inputs.
 *
 * @author Anh Cuong Nguyen
 */
public class JavaMethodCallTracer extends ListenerAdapter{
    boolean traceLib;
    LinkedList<LSCEvent> currentTrace;  // global variable used to log traces
    String failInput;

    public JavaMethodCallTracer(boolean traceLib){
        this.traceLib = traceLib;
        currentTrace = new LinkedList<LSCEvent>();
        failInput = "";
    }

    public String getFailInput(){
        return failInput;
    }

    public LSCEvent[][] trace(String mainClass, String classPath, String inputFilePath){
        LinkedList<LSCEvent[]> dataList = new LinkedList<LSCEvent[]>();
        File inputFile = new File(inputFilePath);
        String input = "";
        try {
            Scanner inputSc = new Scanner(inputFile);
            while (inputSc.hasNext()) {
                /* run the tracer with this particular input */
                input = inputSc.nextLine();
                String[] args = input.split(" ");
                String[] jpfArgs = new String[args.length + 2];
                /* append args with class path and main class */
                System.arraycopy(args, 0, jpfArgs, 2, args.length);
                jpfArgs[0] = "+classpath=lib/jpf-classes.jar;" + classPath;
                jpfArgs[1] = mainClass;
                /* run the tracer */
                currentTrace = new LinkedList<LSCEvent>();  // reset the trace
                Config conf = JPF.createConfig(jpfArgs);
                JPF jpf = new JPF(conf);
                jpf.addListener(this);
                jpf.run();
                dataList.add(currentTrace.toArray(new LSCEvent[currentTrace.size()]));
                System.out.println(currentTrace);
            }
            return dataList.toArray(new LSCEvent[dataList.size()][]);
        } catch (IOException e) {
            failInput = input;
            return null;
        }
    }

    @Override
    public void instructionExecuted(JVM jvm){
        Instruction insn = jvm.getLastInstruction();
        if (insn instanceof InvokeInstruction){
            try {
                InvokeInstruction callInsn = (InvokeInstruction)insn;
                if (traceLib || !isJavaLibCall(callInsn)){
                    String className = callInsn.getMethodInfo().getClassName()
                            + "&" + callInsn.getMethodInfo().getClassInfo().getClassObjectRef();
                    String callName =  callInsn.getInvokedMethod().getReturnTypeName() + " " +
                                        callInsn.getInvokedMethod().getName();
                    callName += "(";
                    String[] callArgs = callInsn.getInvokedMethod().getArgumentTypeNames();
                    for (int inx = 0; inx < callArgs.length; inx++){
                        callName += callArgs[inx];
                        if (inx != callArgs.length-1){
                            callName += ", ";
                        }
                    }
                    callName += ")";
                    String callClassName = callInsn.getInvokedMethodClassName()
                            + "&" + callInsn.getInvokedMethod().getClassInfo().getClassObjectRef();
                    LSCEvent lscEvent = new LSCEvent(className, callClassName, callName);
                    currentTrace.add(lscEvent);
                }
            } catch (Exception e){
            }
        }
    }

    public boolean isJavaLibCall(InvokeInstruction insn){
        String callClass = insn.getInvokedMethodClassName();
        if (   callClass.startsWith("java.applet")
            || callClass.startsWith("java.awt")
            || callClass.startsWith("java.beans")
            || callClass.startsWith("java.io")
            || callClass.startsWith("java.lang")
            || callClass.startsWith("java.math")
            || callClass.startsWith("java.net")
            || callClass.startsWith("java.nio")
            || callClass.startsWith("java.rmi")
            || callClass.startsWith("java.security")
            || callClass.startsWith("java.sql")
            || callClass.startsWith("java.text")
            || callClass.startsWith("java.util")
            || callClass.startsWith("javax.accessibility")
            || callClass.startsWith("javax.activity")
            || callClass.startsWith("javax.crypto")
            || callClass.startsWith("javax.imageio")
            || callClass.startsWith("javax.management")
            || callClass.startsWith("javax.naming")
            || callClass.startsWith("javax.net")
            || callClass.startsWith("javax.print")
            || callClass.startsWith("javax.rmi")
            || callClass.startsWith("javax.security")
            || callClass.startsWith("javax.sound")
            || callClass.startsWith("javax.sql")
            || callClass.startsWith("javax.swing")
            || callClass.startsWith("javax.transaction")
            || callClass.startsWith("javax.xml")
            || callClass.startsWith("sun.misc")
            )
            return true;
        else
            return false;
    }

//    public static void main(String[] args){
//        String[] jpfArgs = {"+classpath=lib/jpf-classes.jar;lib/jpf.jar","lscminer.tracer.HelloWorld"};
//        Config conf = JPF.createConfig(jpfArgs);
//        JPF jpf = new JPF(conf);
//        jpf.addListener(new JavaMethodCallTracer(false));
//        jpf.run();
//    }
}
