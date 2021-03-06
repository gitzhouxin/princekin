#include "filemanager.h"




QString glSpace;
QString space1=" ";
QString space4="    ";
QString space8="        ";
QString space12="            ";
QString equalSign="=";

QString glStrFileHead;
QString glStrFileTail;

QHash<QString,QString> glHashKeyCode;
QHash<QString,QString> glHashVar;



FileManager::FileManager()
{
}



void FileManager::createProject(const QString & arg_newProjectDir)
{
    //创建项目
    //创建模块
    //创建script var invoked check
    QDir mDir(arg_newProjectDir);
    if(mDir.exists())
    {
        QMessageBox::information(NULL,"提示", "当前项目已存在");
        return;
    }
    else
    {
         mDir.mkdir(arg_newProjectDir);
         /*
         mDir.mkdir(newProjectDir + QDir::separator() + "模块");
         mDir.mkdir(newProjectDir + QDir::separator() + "模块" + QDir::separator() + "check");
         mDir.mkdir(newProjectDir + QDir::separator() + "模块" + QDir::separator() + "invoked");
         mDir.mkdir(newProjectDir + QDir::separator() + "模块" + QDir::separator() + "script");
         mDir.mkdir(newProjectDir + QDir::separator() + "模块" + QDir::separator() + "var");
         */
    }
}

bool FileManager::createModule(const QString & arg_selectTreePath,const QString & arg_inputName)
{
    QString mSelectTreePath=QDir::toNativeSeparators(arg_selectTreePath);
    QDir mDir(mSelectTreePath + QDir::separator() + arg_inputName);
    if(mDir.exists())
    {
        return false;
    }
    else
    {
         mDir.mkdir(mSelectTreePath + QDir::separator() + arg_inputName);
    }


    QDir mDir2(gUiautomatorDir + QDir::separator() + arg_inputName);
    if(mDir2.exists())
    {
        return false;
    }
    else
    {
         mDir2.mkdir(gUiautomatorDir + QDir::separator() + arg_inputName);
    }
    return true;
}

bool FileManager::createSuiteModule(const QString & arg_selectTreePath,const QString & arg_inputName)
{
    QString mSelectTreePath=QDir::toNativeSeparators(arg_selectTreePath);
    QDir mDir(mSelectTreePath + QDir::separator() + arg_inputName);
    if(mDir.exists())
    {
        return false;
    }
    else
    {
         mDir.mkdir(mSelectTreePath + QDir::separator() + arg_inputName);
    }

    return true;
}


bool FileManager::createScript(const QString & arg_selectTreePath,const QString & arg_inputName)
{
    QString mSelectTreePath=QDir::toNativeSeparators(arg_selectTreePath);
    QFile file(mSelectTreePath + QDir::separator() + arg_inputName + ".txt");
    if(file.exists())
    {
        file.close();
        return false;
    }
    file.open(QIODevice::WriteOnly);
    file.close();
    return true;
}




void FileManager::setCurrentOpenFile(const QString &arg_currentOpenFile)
{
    currentOpenFile=arg_currentOpenFile;
}

void FileManager::setLoopNumber(int arg_loopNumber)
{
    loopNumber=arg_loopNumber;
}


void FileManager::scriptToUiautomator(const QString &arg_file)//arg_file是文件路径
{
    QStringList mList=parsePath(arg_file);

    //"\\tesstcase\\script\\play\\a.txt"
    //"" tesstcase script play a.txt



    if(glStrFileHead==NULL)
    {
        glStrFileHead=readAllFile(gConfigDir + QDir::separator() + "filehead.java");
    }
    if(glStrFileTail==NULL)
    {
        glStrFileTail=readAllFile(gConfigDir + QDir::separator() + "filetail.java");
    }
    if(glHashKeyCode.count()==0)
    {
        readKeyCode(gConfigDir + QDir::separator() + "keycode.txt");//没有这个文件
    }

    //*****读var********//
    readVar(gVarDir + QDir::separator() + "var.txt");

    //转换临时文件-invoke
    QStringList mTempScriptList=convertScriptToTemp(arg_file);


    //"E:/repository/testcase/script/play/a/a.txt"
    //"E:/repository/testcase/script/play/a"
    QFileInfo mFileInfo(arg_file);
    QString mAbsolutePath=mFileInfo.absolutePath();

    QDir mDir(mAbsolutePath);
    QString mParentName=mDir.dirName();

    QString mUiautomatorPath=gUiautomatorDir + QDir::separator() + mParentName;
    mUiautomatorPath=QDir::toNativeSeparators(mUiautomatorPath);
    createPath(mUiautomatorPath);

    baseName=getBaseName(arg_file);

    QString mUiautomatorJavaFile=mUiautomatorPath + QDir::separator() + baseName + ".java";
    createJava(mUiautomatorJavaFile);

    createUiautomator(mUiautomatorJavaFile,mTempScriptList);
}


void FileManager::createUiautomator(const QString &arg_javafile,QStringList arg_tempScriptList)
{
    QFile mFile(arg_javafile);
    if(!mFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        mFile.close();
        return;
    }

    QString mStr;

    stream=new QTextStream(&mFile);

    //QTextCodec *tc=QTextCodec::codecForName("GBK");
    stream->setCodec(tc);

    *stream<<glStrFileHead;

    mStr="public class " + baseName + " extends UiAutomatorTestCase";
    *stream<<mStr<<endl;
    *stream<<"{"<<endl;
        testDemo(arg_tempScriptList);
    *stream<<glStrFileTail<<endl;

    mFile.close();
    delete stream;
}


void FileManager::testDemo(QStringList arg_tempScriptList)
{
    if(loopNumber<=1)
    {
        glSpace=space8;

        *stream<<space4 + "String fileName=" + "\"" +baseName + "\";"        <<endl;

        *stream<<space4 + "public void testDemo()"<<endl;
        *stream<<space4 + "{"<<endl;
            *stream<<space8 + "device=getUiDevice();"<<endl;
            *stream<<space8 + "monitor();"<<endl;
            *stream<<space8 + "Bundle bundle=getParams();"<<endl;
            *stream<<space8 + "checkStatStr=bundle.getString(\"checkStat\");"<<endl;
            *stream<<space8 + "appnameStr=bundle.getString(\"appname\");"<<endl;

            parseTempScript(arg_tempScriptList);
        *stream<<space4 + "}"<<endl;
    }
    else
    {
        glSpace=space12;

        *stream<<space4 + "String fileName=" + "\"" +baseName + "\";"        <<endl;
        *stream<<space4 + "public void testDemo()"<<endl;
        *stream<<space4 + "{"<<endl;
            *stream<<space8 + "device=getUiDevice();"<<endl;
            //for(int i=0;i<10;i++)
            *stream<<space8 + "for(int i=0;i<" + QString::number(loopNumber) + ";i++)"<<endl;
            *stream<<space8 + "{"<<endl;
                parseTempScript(arg_tempScriptList);//这里用回到上面的glspace
            *stream<<space8 + "}"<<endl;
        *stream<<space4 + "}"<<endl;
    }
}

void FileManager::parseTempScript(QStringList arg_tempScriptList)
{
    foreach (QString mStr, arg_tempScriptList)
    {
        if(mStr.trimmed().startsWith("back"))
        {
            pressBack(mStr);
        }
        else if(mStr.trimmed().startsWith("fastBack"))
        {
            fastBack(mStr);
        }
        else if(mStr.trimmed().startsWith("home"))
        {
            pressHome(mStr);
        }
        else if(mStr.trimmed().startsWith("menu"))
        {
            pressMenu(mStr);
        }



        else if(mStr.trimmed().startsWith("pushApk "))
        {
            pushApk("pushApk",mStr,strInstallOK,space1);
        }
        else if(mStr.trimmed().startsWith("pushApk="))
        {
            pushApk("pushApk",mStr,strInstallOK,equalSign);
        }




        else if(mStr.trimmed().startsWith("install "))
        {
            unOrInstallOrStart("install",mStr,strInstallOK,space1);
        }
        else if(mStr.trimmed().startsWith("install="))
        {
            unOrInstallOrStart("install",mStr,strInstallOK,equalSign);
        }

        else if(mStr.trimmed().startsWith("uninstall "))
        {
            unOrInstallOrStart("uninstall",mStr,strUninstallOK,space1);
        }
        else if(mStr.trimmed().startsWith("uninstall="))
        {
            unOrInstallOrStart("uninstall",mStr,strUninstallOK,equalSign);
        }


        else if(mStr.trimmed().startsWith("startApp "))
        {
            unOrInstallOrStart("startApp",mStr,strStartOK,space1);
        }
        else if(mStr.trimmed().startsWith("startApp="))
        {
            unOrInstallOrStart("startApp",mStr,strStartOK,equalSign);
        }

        else if(mStr.trimmed().startsWith("checkStatPoint "))
        {
            checkStatPoint(mStr);
        }


        else if(mStr.trimmed().startsWith("clickText "))
        {
            createString("clickText",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickText="))
        {
            createString("clickText",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickText_notExit "))
        {
            createStringNotExit("clickText_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickText_notExit="))
        {
            createStringNotExit("clickText_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("clickTextMatches "))
        {
            createString("clickTextMatches",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextMatches="))
        {
            createString("clickTextMatches",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickTextMatches_notExit "))
        {
            createStringNotExit("clickTextMatches_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextMatches_notExit="))
        {
            createStringNotExit("clickTextMatches_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("clickTextStartWith "))
        {
            createString("clickTextStartWith",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextStartWith="))
        {
            createString("clickTextStartWith",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickTextStartWith_notExit "))
        {
            createStringNotExit("clickTextStartWith_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextStartWith_notExit="))
        {
            createStringNotExit("clickTextStartWith_notExit",mStr,equalSign);
        }





        else if(mStr.trimmed().startsWith("clickTextContains "))
        {
            createString("clickTextContains",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextContains="))
        {
            createString("clickTextContains",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickTextContains_notExit "))
        {
            createStringNotExit("clickTextContains_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickTextContains_notExit="))
        {
            createStringNotExit("clickTextContains_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("clickDescription "))
        {
            createString("clickDescription",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescription="))
        {
            createString("clickDescription",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickDescription_notExit "))
        {
            createStringNotExit("clickDescription_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescription_notExit="))
        {
            createStringNotExit("clickDescription_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("clickDescriptionMatches "))
        {
            createString("clickDescriptionMatches",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionMatches="))
        {
            createString("clickDescriptionMatches",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionMatches_notExit "))
        {
            createStringNotExit("clickDescriptionMatches_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionMatches_notExit="))
        {
            createStringNotExit("clickDescriptionMatches_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("clickDescriptionStartWith "))
        {
            createString("clickDescriptionStartWith",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionStartWith="))
        {
            createString("clickDescriptionStartWith",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionStartWith_notExit "))
        {
            createStringNotExit("clickDescriptionStartWith_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionStartWith_notExit="))
        {
            createStringNotExit("clickDescriptionStartWith_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("clickDescriptionContains "))
        {
            createString("clickDescriptionContains",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionContains="))
        {
            createString("clickDescriptionContains",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionContains_notExit "))
        {
            createStringNotExit("clickDescriptionContains_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickDescriptionContains_notExit="))
        {
            createStringNotExit("clickDescriptionContains_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("clickResourceId "))
        {
            createString("clickResourceId",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickResourceId="))
        {
            createString("clickResourceId",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickResourceId_notExit "))
        {
            createStringNotExit("clickResourceId_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickResourceId_notExit="))
        {
            createStringNotExit("clickResourceId_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("clickIdOrClass "))
        {
            clickIdOrClass("clickIdOrClass",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickIdOrClass="))
        {
            clickIdOrClass("clickIdOrClass",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickIdOrClass_notExit "))
        {
            clickIdOrClassNotExit("clickIdOrClass_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickIdOrClass_notExit="))
        {
            clickIdOrClassNotExit("clickIdOrClass_notExit",mStr,equalSign);
        }






















        else if(mStr.trimmed().startsWith("clickResourceIdMatches "))
        {
            createString("clickResourceIdMatches",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickResourceIdMatches="))
        {
            createString("clickResourceIdMatches",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickResourceIdMatches_notExit "))
        {
            createStringNotExit("clickResourceIdMatches_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickResourceIdMatches_notExit="))
        {
            createStringNotExit("clickResourceIdMatches_notExit",mStr,equalSign);
        }





        else if(mStr.trimmed().startsWith("clickClassName "))
        {
            createString("clickClassName",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickClassName="))
        {
            createString("clickClassName",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickClassName_notExit "))
        {
            createStringNotExit("clickClassName_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickClassName_notExit="))
        {
            createStringNotExit("clickClassName_notExit",mStr,equalSign);
        }





        else if(mStr.trimmed().startsWith("clickClassNameMatches "))
        {
            createString("clickClassNameMatches",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickClassNameMatches="))
        {
            createString("clickClassNameMatches",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickClassNameMatches_notExit "))
        {
            createStringNotExit("clickClassNameMatches_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickClassNameMatches_notExit="))
        {
            createStringNotExit("clickClassNameMatches_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("clickFocused "))
        {
            createIntBool("clickFocused",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickFocused="))
        {
            createIntBool("clickFocused",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickFocused_notExit "))
        {
            createIntBoolNotExit("clickFocused_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickFocused_notExit="))
        {
            createIntBoolNotExit("clickFocused_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("clickFocusable "))
        {
            createIntBool("clickFocusable",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("clickFocusable="))
        {
            createIntBool("clickFocusable",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("clickFocusable_notExit "))
        {
            createIntBoolNotExit("clickFocusable_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("clickFocusable_notExit="))
        {
            createIntBoolNotExit("clickFocusable_notExit",mStr,equalSign);
        }

















        else if(mStr.trimmed().startsWith("checkText "))
        {
            createString("checkText",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkText="))
        {
            createString("checkText",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkText_notExit "))
        {
            createStringNotExit("checkText_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkText_notExit="))
        {
            createStringNotExit("checkText_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("checkTextMatches "))
        {
            createString("checkTextMatches",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextMatches="))
        {
            createString("checkTextMatches",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkTextMatches_notExit "))
        {
            createStringNotExit("checkTextMatches_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextMatches_notExit="))
        {
            createStringNotExit("checkTextMatches_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("checkTextStartWith "))
        {
            createString("checkTextStartWith",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextStartWith="))
        {
            createString("checkTextStartWith",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkTextStartWith_notExit "))
        {
            createStringNotExit("checkTextStartWith_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextStartWith_notExit="))
        {
            createStringNotExit("checkTextStartWith_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("checkTextContains "))
        {
            createString("checkTextContains",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextContains="))
        {
            createString("checkTextContains",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkTextContains_notExit "))
        {
            createStringNotExit("checkTextContains_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextContains_notExit="))
        {
            createStringNotExit("checkTextContains_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("checkTextSelected "))
        {
            createString("checkTextSelected",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextSelected="))
        {
            createString("checkTextSelected",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkTextSelected_notExit "))
        {
            createStringNotExit("checkTextSelected_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextSelected_notExit="))
        {
            createStringNotExit("checkTextSelected_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("checkTextChecked "))
        {
            createString("checkTextChecked",mStr,strCheckOK,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextChecked="))
        {
            createString("checkTextChecked",mStr,strCheckOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("checkTextChecked_notExit "))
        {
            createStringNotExit("checkTextChecked_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("checkTextChecked_notExit="))
        {
            createStringNotExit("checkTextChecked_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("locateClassName "))
        {
            createStringNotExit("locateClassName",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("locateClassName="))
        {
            createStringNotExit("locateClassName",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("locateClassNameMatches "))
        {
            createStringNotExit("locateClassNameMatches",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("locateClassNameMatches="))
        {
            createStringNotExit("locateClassNameMatches",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("locateScrollable "))
        {
            createIntBoolNotExit("locateScrollable",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("locateScrollable="))
        {
            createIntBoolNotExit("locateScrollable",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("scrollTextIntoView "))
        {
            createString("scrollTextIntoView",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollTextIntoView="))
        {
            createString("scrollTextIntoView",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollTextIntoView_notExit "))
        {
            createStringNotExit("scrollTextIntoView_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollTextIntoView_notExit="))
        {
            createStringNotExit("scrollTextIntoView_notExit",mStr,equalSign);
        }





        else if(mStr.trimmed().startsWith("scrollIdOrClass "))
        {
            scrollIdOrClass("scrollIdOrClass",mStr,strClickOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollIdOrClass="))
        {
            scrollIdOrClass("scrollIdOrClass",mStr,strClickOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollIdOrClass_notExit "))
        {
            scrollIdOrClassNotExit("scrollIdOrClass_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollIdOrClass_notExit="))
        {
            scrollIdOrClassNotExit("scrollIdOrClass_notExit",mStr,equalSign);
        }











        else if(mStr.trimmed().startsWith("scrollDescriptionIntoView "))
        {
            createString("scrollDescriptionIntoView",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollDescriptionIntoView="))
        {
            createString("scroll_DescriptionIntoView",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollDescriptionIntoView_notExit "))
        {
            createStringNotExit("scrollDescriptionIntoView_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollDescriptionIntoView_notExit="))
        {
            createStringNotExit("scrollDescriptionIntoView_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed()=="flingForward")
        {
            createNoParameter("flingForward",strScrollOK);
        }
        else if(mStr.trimmed()=="flingForward_notExit")
        {
            createNoParameterNotExit("flingForward_notExit");
        }


        else if(mStr.trimmed()=="flingBackward")
        {
            createNoParameter("flingBackward",strScrollOK);
        }
        else if(mStr.trimmed()=="flingBackward_notExit")
        {
            createNoParameterNotExit("flingBackward_notExit");
        }



        else if(mStr.trimmed()=="scrollForward")
        {
            createNoParameter("scrollForward",strScrollOK);
        }
        else if(mStr.trimmed()=="scrollForward_notExit")
        {
            createNoParameterNotExit("scrollForward_notExit");
        }
        else if(mStr.trimmed().startsWith("scrollForward "))
        {
            createIntBool("scrollForward",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollForward="))
        {
            createIntBool("scrollForward",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollForward_notExit "))
        {
            createIntBoolNotExit("scrollForward_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollForward_notExit="))
        {
            createIntBoolNotExit("scrollForward_notExit",mStr,equalSign);
        }





        else if(mStr.trimmed()=="scrollBackward")
        {
            createNoParameter("scrollBackward",strScrollOK);
        }
        else if(mStr.trimmed()=="scrollBackward_notExit")
        {
            createNoParameterNotExit("scrollBackward_notExit");
        }
        else if(mStr.trimmed().startsWith("scrollBackward "))
        {
            createIntBool("scrollBackward",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollBackward="))
        {
            createIntBool("scrollBackward",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollBackward_notExit "))
        {
            createIntBoolNotExit("scrollBackward_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollBackward_notExit="))
        {
            createIntBoolNotExit("scrollBackward_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("flingToEnd "))
        {
            createIntBool("flingToEnd",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("flingToEnd="))
        {
            createIntBool("flingToEnd",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("flingToEnd_notExit "))
        {
            createIntBoolNotExit("flingToEnd_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("flingToEnd_notExit="))
        {
            createIntBoolNotExit("flingToEnd_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("flingToBeginning "))
        {
            createIntBool("flingToBeginning",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("flingToBeginning="))
        {
            createIntBool("flingToBeginning",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("flingToBeginning_notExit "))
        {
            createIntBoolNotExit("flingToBeginning_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("flingToBeginning_notExit="))
        {
            createIntBoolNotExit("flingToBeginning_notExit",mStr,equalSign);
        }




        else if(mStr.trimmed().startsWith("scrollToEnd "))
        {
            createIntBool("scrollToEnd",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd="))
        {
            createIntBool("scrollToEnd",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd_notExit "))
        {
            createIntBoolNotExit("scrollToEnd_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd_notExit="))
        {
            createIntBoolNotExit("scrollToEnd_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("scrollToEnd2 "))
        {

            //createIntBool_scroll("scrollToEnd2",mStr,strScrollOK,space1);
            createIntBool("scrollToEnd2",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd2= "))
        {
            //createIntBool_scroll("scrollToEnd2",mStr,strScrollOK,equalSign);
            createIntBool("scrollToEnd2",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd2_notExit "))
        {
            //createIntBool_scrollNotReturn("scrollToEnd2_notExit",mStr,space1);
            createIntBoolNotExit("scrollToEnd2_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToEnd2_notExit="))
        {
            //createIntBool_scrollNotReturn("scrollToEnd2_notExit",mStr,equalSign);
            createIntBoolNotExit("scrollToEnd2_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("scrollToBeginning "))
        {
            createIntBool("scrollToBeginning",mStr,strScrollOK,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToBeginning="))
        {
            createIntBool("scrollToBeginning",mStr,strScrollOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("scrollToBeginning_notExit "))
        {
            createIntBoolNotExit("scrollToBeginning_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToBeginning_notExit="))
        {
            createIntBoolNotExit("scrollToBeginning_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed().startsWith("scrollToBeginning2 "))
        {
            //createIntBool_scroll("scrollToBeginning2",mStr,strScrollOK,space1);
            createIntBool("scrollToBeginning2",mStr,strScrollOK,space1);

        }
        else if(mStr.trimmed().startsWith("scrollToBeginning2= "))
        {
            //createIntBool_scroll("scrollToBeginning2",mStr,strScrollOK,equalSign);
            createIntBool("scrollToBeginning2",mStr,strScrollOK,equalSign);
        }


        else if(mStr.trimmed().startsWith("scrollToBeginning2_notExit "))
        {
            //createIntBool_scrollNotReturn("scrollToBeginning2_notExit",mStr,space1);
            createIntBoolNotExit("scrollToBeginning2_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("scrollToBeginning2_notExit="))
        {
            //createIntBool_scrollNotReturn("scrollToBeginning2_notExit",mStr,equalSign);
            createIntBoolNotExit("scrollToBeginning2_notExit",mStr,equalSign);
        }



        else if(mStr.trimmed().startsWith("inputText "))
        {
            createString("inputText",mStr,strOK,space1);
        }
        else if(mStr.trimmed().startsWith("inputText="))
        {
            createString("inputText",mStr,strOK,equalSign);
        }
        else if(mStr.trimmed().startsWith("inputText_notExit "))
        {
            createStringNotExit("inputText_notExit",mStr,space1);
        }
        else if(mStr.trimmed().startsWith("inputText_notExit="))
        {
            createStringNotExit("inputText_notExit",mStr,equalSign);
        }


        else if(mStr.trimmed()=="setHorizontal")
        {
            createNoParameterNotExit("setHorizontal");
        }
        else if(mStr.trimmed()=="setVertical")
        {
            createNoParameterNotExit("setVertical");
        }
        else if(mStr.trimmed()==("clearText"))
        {
            createNoParameter(mStr,strOK);
        }
        else if(mStr.trimmed()==("clearText_notExit"))
        {
            createNoParameterNotExit(mStr);
        }




        else if(mStr.trimmed().startsWith("println "))
        {
            sohuPrint(mStr,space1);
        }
        else if(mStr.trimmed().startsWith("println="))
        {
            sohuPrint(mStr,equalSign);
        }

        else if(mStr.trimmed()=="takeScreen")
        {
            takeScreen("take_Screen",baseName);
        }

        else if(mStr.trimmed()=="takeScreenError")
        {
            takeScreen("take_ScreenError",baseName);
        }


        else if(mStr.trimmed().startsWith("sleep "))
        {
            sleep(mStr,space1);
        }
        else if(mStr.trimmed().startsWith("sleep="))
        {
            sleep(mStr,equalSign);
        }
        else
        {
            otherLine(mStr);
        }

    }
}



void FileManager::pressBack(const QString &arg_line)
{
    QString mStr;
    if(arg_line.trimmed()=="back")
    {
        mStr=glSpace + "press_Back();";
    }
    else
    {
        //mStr=glSpace + "//" + arg_line + "不符合规范";
        mStr=glSpace + arg_line;
    }
    *stream<<mStr<<endl;
}

void FileManager::fastBack(const QString &arg_line)
{
    QString mStr;
    if(arg_line.trimmed()=="fastBack")
    {
        mStr=glSpace + "fastBack();";
    }
    else
    {
        //mStr=glSpace + "//" + arg_line + "不符合规范";
        mStr=glSpace + arg_line;
    }
    *stream<<mStr<<endl;
}

void FileManager::pressHome(const QString &arg_line)
{
    QString mStr;
    if(arg_line.trimmed()=="home")
    {
        mStr=glSpace + "press_Home();";
    }
    else
    {
        //mStr=glSpace + "//" + arg_line + "不符合规范";
        mStr=glSpace + arg_line;
    }
    *stream<<mStr<<endl;
}




void FileManager::pressMenu(const QString &arg_line)
{
    QString mStr;
    if(arg_line.trimmed()=="menu")
    {
        mStr=glSpace + "press_Menu();";
    }
    else
    {
        //mStr=glSpace + "//" + arg_line + "不符合规范";
        mStr=glSpace + arg_line;
    }
    *stream<<mStr<<endl;
}

//******************************clickText checkText*********************************************//
void FileManager::createString(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }



}


void FileManager::createStringNotExit(const QString &arg_funName,const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
        }
        else if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
            *stream<<mStr<<endl;
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}



void FileManager::createIntBool(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "("  + mListResult.at(1) +");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + mListResult.at(1) + "," + mListResult.at(2) +");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;

            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "("  + mVarValue  +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();
                mStr=glSpace + arg_funName + "(" + mVarValue1 + "," + mVarValue2 +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}


void FileManager::createIntBoolNotExit(const QString &arg_funName,const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "("  + mListResult.at(1) +");";
            *stream<<mStr<<endl;
        }
        else if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + mListResult.at(1) + "," + mListResult.at(2) +");";
            *stream<<mStr<<endl;
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;

            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "("  + mVarValue  +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();
                mStr=glSpace + arg_funName + "(" + mVarValue1 + "," + mVarValue2 +");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}


void FileManager::createNoParameter(const QString &arg_funName,const QString &arg_isok)
{
    QString mStr;
    mStr=glSpace + arg_funName + "();";

    *stream<<mStr<<endl;
    writeOK(arg_isok);
}
void FileManager::createNoParameterNotExit(const QString &arg_funName)
{
    QString mStr;
    mStr=glSpace + arg_funName + "();";

    *stream<<mStr<<endl;
}


void FileManager::takeScreen(const QString &arg_funName,const QString &arg_baseName)
{
    QString mStr;
    mStr=glSpace + arg_funName + "(" + "\"" + arg_baseName + "\"" +");";

    *stream<<mStr<<endl;
}













void FileManager::createIntBool_scroll(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + mListResult.at(1) + "," + mListResult.at(2) + ");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);

            mVarValue1=glHashVar.value(mVarName1);
            mVarValue2=glHashVar.value(mVarName2);

            if(mVarValue1.isEmpty() || mVarValue2.isEmpty())
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + arg_funName + "(" + mVarValue1 + "," + mVarValue2 + ");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}



void FileManager::createIntBool_scrollNotReturn(const QString &arg_funName,const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==3)
        {
            mStr=glSpace + arg_funName + "(" + mListResult.at(1) + "," + mListResult.at(2) + ");";
            *stream<<mStr<<endl;
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);

            mVarValue1=glHashVar.value(mVarName1);
            mVarValue2=glHashVar.value(mVarName2);

            if(mVarValue1.isEmpty() || mVarValue2.isEmpty())
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + arg_funName + "(" + mVarValue1 + "," + mVarValue2 + ");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}










void FileManager::writeOK(const QString &arg_isok)
{
    QString mStr=glSpace + "if(!" + arg_isok + ")";
    *stream<<mStr<<endl;

    mStr=glSpace + "{";
    *stream<<mStr<<endl;


    mStr=space12 + "take_ScreenError(fileName);";
    *stream<<mStr<<endl;


    mStr=glSpace + "    return;";
    *stream<<mStr<<endl;

    mStr=glSpace + "}";
    *stream<<mStr<<endl;
}

void FileManager::writeOKStart(const QString &arg_isok)
{
    QString mStr=glSpace + "if(!" + arg_isok + ")";
    *stream<<mStr<<endl;

    mStr=glSpace + "{";
    *stream<<mStr<<endl;


    mStr=space12 + "take_ScreenError(fileName);";
    *stream<<mStr<<endl;

    mStr=glSpace + "}";
    *stream<<mStr<<endl;
}


void FileManager::pushApk(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();
    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            //mStr=glSpace + "System.out.println(" + "\"" + "pushApk=" + mListResult.at(1) + "\"" + ");";
            //*stream<<mStr<<endl;
            //writeOK(arg_isok);

        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;

                //mStr=glSpace + "System.out.println(" + "\"" + "pushApk=" + mVarValue + "\"" + ");";
                //*stream<<mStr<<endl;
                //writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}




void FileManager::unOrInstallOrStart(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();
    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            writeOK(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}
/*
void FileManager::startApp(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();
    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            //writeOKStart(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
                //writeOKStart(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}
*/
void FileManager::checkStartApp(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();
    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            //writeOKStart(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
                //writeOKStart(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}


void FileManager::startApp(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();
    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + arg_funName + "(" + "\"" + mListResult.at(1) + "\"" +");";
            *stream<<mStr<<endl;
            writeOKStart(arg_isok);
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + arg_funName + "(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
                writeOKStart(arg_isok);
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}



















void FileManager::sohuPrint(const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + "System.out.println(" + "\"" + mListResult.at(1) + "\"" +");";
        }
        else
        {
            mStr=glSpace + arg_line;
        }
        *stream<<mStr<<endl;
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;

            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + "System.out.println(" + "\"" + mVarValue + "\"" +");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
}




void FileManager::sleep(const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mStr=glSpace + "sleep("  + mListResult.at(1) + ");";
            *stream<<mStr<<endl;
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                mStr=glSpace + "sleep("  + mVarValue + ");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }




    /*
    bool mOk;
    int mCount;
    QString mStr;
    QStringList mListResult;

    mListResult=getSpaceLine(arg_line);
    mCount=mListResult.count();
    if(mCount==2)
    {
        int dec=mListResult.at(1).toInt(&mOk);
        if(mOk)
        {
            mStr=glSpace + "sleep("  + mListResult.at(1) + ");";
        }
        else
        {
            mStr=glSpace + arg_line;
        }
        *stream<<mStr<<endl;
    }
    else
    {
        mStr=glSpace + arg_line;
        *stream<<mStr<<endl;
    }
    */
}











void FileManager::otherLine(const QString &arg_line)
{
    *stream<<glSpace + arg_line<<endl;
}



QStringList FileManager::getSplist(const QString &arg_line,const QString &arg_fgf)
{
    QString mStr;
    QStringList mList;

    mStr=arg_line.trimmed();
    mList=mStr.split(arg_fgf,QString::SkipEmptyParts);
    return mList;
}




QStringList FileManager::getSpaceLine(const QString &arg_line)
{
    QString mStr;
    QStringList mList;

    mStr=arg_line.trimmed();
    mList=mStr.split(" ");
    return mList;

    /*
    //clickText
    int mLen;
    int mIndex;

    QString mStr;
    QString mStr1;

    mStr=arg_line.trimmed();
    mLen=mStr.length();
    mIndex=mStr.indexOf(space1);
    if(mIndex==-1)
    {
    }
    else
    {
        mStr1=mStr.right(mLen-mIndex-1).trimmed();
    }
    return mStr1;
    */
}










QStringList FileManager::convertScriptToTemp(const QString &arg_scriptFile)
{
    QStringList mList;
    QFile mFile(arg_scriptFile);
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return mList;
    }

    QString mLine;
    QString mTrimLine;
    QStringList mResultList;

    QTextStream mStream(&mFile);
    mStream.setCodec(tc);//打开非录制模块以为的模块再打开录制模块编译时，由于编码方式改变了，导致读到的汉字是乱码，所以这里小范围规定编码方式 hechang 20170724
    while(!mStream.atEnd())
    {
        mLine=mStream.readLine();
        mTrimLine=mLine.trimmed();
        if(mTrimLine.startsWith("invoke="))
        {
            mResultList=parseInvoke(mTrimLine);
            mList.append(mResultList);
        }
        else
        {
            mList.append(mLine);
        }
    }
    mFile.close();
    return mList;
}


QStringList FileManager::parseInvoke(const QString & arg_line)//arg_line:invoke=resume这句话
{
    QString mStr;
    QString mInvokeFileName;
    QStringList mList;

    mList=arg_line.split("=");//这里一步有用吗？如果没错就清空了，如果有错编译不会通过，下面写的注释就只是提示。
    mInvokeFileName=mList.at(1).trimmed();

    mStr=gInvokeDir + QDir::separator() + mInvokeFileName + ".txt";
    QFile mFile(mStr);
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mStr="//" + arg_line + "调用失败";//这里的意图是如果调用invoke有错还能继续编译吗？如果是，见上一句注释
        mList.append(mStr);
        return mList;
    }

    mList.clear();
    QTextStream mStream(&mFile);
    mStream.setCodec(tc);
    while(!mStream.atEnd())
    {
        mStr=mStream.readLine();
        mList.append(mStr);
    }
    mFile.close();
    return mList;
}


QString FileManager::readAllFile(const QString &arg_head)
{
    QFile mFile(arg_head);
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mFile.close();
        return NULL;
    }
    QTextStream mStream(&mFile);
    QString mStr=mStream.readAll();
    mFile.close();
    return mStr;
}

void FileManager::readKeyCode(const QString &arg_code)
{
    QFile mFile(arg_code);
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mFile.close();
        return;
    }

    QString mLine;
    QStringList mSplistResult;
    QTextStream mStream(&mFile);
    while(!mStream.atEnd())
    {
        mLine=mStream.readLine();
        if(mLine.contains("="))
        {
            mSplistResult=mLine.split("=");
            glHashKeyCode.insert(mSplistResult.at(0),mSplistResult.at(1));
        }
    }
    mFile.close();
}




void FileManager::readVar(const QString &arg_var)
{
    glHashVar.clear();

    QFile mFile(arg_var);
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mFile.close();
        return;
    }

    QString line;
    QStringList splistResult;
    QTextStream mStream(&mFile);
    while(!mStream.atEnd())
    {
        line=mStream.readLine();
        if(line.contains("="))
        {
            splistResult=line.split("=");
            glHashVar.insert(splistResult.at(0),splistResult.at(1));
        }
    }
    mFile.close();
}











void FileManager::createJava(const QString &arg_javafile)
{
    QFile file(arg_javafile);
    if(file.exists())
    {
        file.close();
        return;
    }
    file.open(QIODevice::WriteOnly);
    file.close();
}

void FileManager::createPath(const QString &arg_path)
{
    QDir mDir(arg_path);
    if(mDir.exists())
    {
    }
    else
    {
        mDir.mkpath(arg_path);
    }
}


QStringList FileManager::parsePath(const QString &arg_path)
{
    //"E:\\repository
    //"E:\\repository\\testcase\\script\\play\\a.txt"
    int len1=gRepositoryDir.length();
    int len2=arg_path.length();
    QString mRightStr=arg_path.right(len2-len1);
    //"testcase\\script\\play\\a.txt"
    //"" testcase script play a.txt
    QStringList mList=mRightStr.split(QDir::separator());
    return mList;
}

QString FileManager::getBaseName(const QString &currentOpenFile)
{
    QFileInfo info(currentOpenFile);
    return info.baseName();
}

QString FileManager::getSubPath(const QString &arg_currentOpenFile)
{
    int len1=gScriptDir.length();
    int len2=arg_currentOpenFile.length();
    QString mSubPath="";
    return mSubPath;
}



bool FileManager::isD(const QString &arg_path)
{
    QFileInfo info(arg_path);
    if(info.isDir())
    {
        return true;
    }
    else
    {
        return false;
    }
}



void FileManager::clickIdOrClass(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QString mSSS;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "clickResourceId" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + "clickClassName" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else if(mCount==3)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "clickResourceId" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + "clickClassName" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                if(mVarValue.contains(":id"))
                {
                    mStr=glSpace + "clickResourceId" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
                else
                {
                    mStr=glSpace + "clickClassName" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                if(mVarValue1.contains(":id"))
                {
                    mStr=glSpace + "clickResourceId" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
                else
                {
                    mStr=glSpace + "clickClassName" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}

void FileManager::clickIdOrClassNotExit(const QString &arg_funName,const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QString mSSS;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "clickResourceId_notExit" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + "clickClassName_notExit" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "clickResourceId_notExit" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + "clickClassName_notExit" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                if(mVarValue.contains(":id"))
                {
                    mStr=glSpace + "clickResourceId_notExit" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                }
                else
                {
                    mStr=glSpace + "clickClassName_notExit" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                }
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                if(mVarValue1.contains(":id"))
                {
                    mStr=glSpace + "clickResourceId_notExit" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                }
                else
                {
                    mStr=glSpace + "clickClassName_notExit" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                }
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}



void FileManager::scrollIdOrClass(const QString &arg_funName,const QString &arg_line,const QString &arg_isok,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QString mSSS;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                //createString("clickIdOrClass",mStr,strClickOK,space1);
                mStr=glSpace + "scrollId" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + "scrollClass" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else if(mCount==3)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "scrollId" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
            else
            {
                mStr=glSpace + "scrollClass" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
                writeOK(arg_isok);
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                if(mVarValue.contains(":id"))
                {
                    mStr=glSpace + "scrollId" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
                else
                {
                    mStr=glSpace + "scrollClass" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                if(mVarValue1.contains(":id"))
                {
                    mStr=glSpace + "scrollId" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
                else
                {
                    mStr=glSpace + "scrollClass" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                    writeOK(arg_isok);
                }
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}

void FileManager::scrollIdOrClassNotExit(const QString &arg_funName,const QString &arg_line,const QString &arg_fgf)
{
    int mCount;
    QString mStr;
    QString mSSS;
    QStringList mListResult;

    mListResult=getSplist(arg_line,arg_fgf);
    mCount=mListResult.count();

    if(arg_fgf==space1)
    {
        if(mCount==2)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "scrollId_notExit" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + "scrollClass_notExit" + "(" + "\"" + mListResult.at(1) + "\"" +");";
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            mSSS=mListResult.at(1);
            if(mSSS.contains(":id"))
            {
                mStr=glSpace + "scrollId_notExit" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
            }
            else
            {
                mStr=glSpace + "scrollClass_notExit" + "(" + "\"" + mListResult.at(1) + "\"" + "," + mListResult.at(2) +");";
                *stream<<mStr<<endl;
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }
    else if(arg_fgf==equalSign)
    {
        if(mCount==2)
        {
            QString mVarName;
            QString mVarValue;
            mVarName=mListResult.at(1);
            if(glHashVar.contains(mVarName))
            {
                mVarValue=glHashVar.value(mVarName).trimmed();
                if(mVarValue.contains(":id"))
                {
                    mStr=glSpace + "scrollId_notExit" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                }
                else
                {
                    mStr=glSpace + "scrollClass_notExit" + "(" + "\"" + mVarValue + "\"" +");";
                    *stream<<mStr<<endl;
                }
            }
            else
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
        }
        else if(mCount==3)
        {
            QString mVarName1;
            QString mVarName2;
            QString mVarValue1;
            QString mVarValue2;

            mVarName1=mListResult.at(1);
            mVarName2=mListResult.at(2);
            if(!glHashVar.contains(mVarName1) || !glHashVar.contains(mVarName2))
            {
                mStr=glSpace + arg_line;
                *stream<<mStr<<endl;
            }
            else
            {
                mVarValue1=glHashVar.value(mVarName1).trimmed();
                mVarValue2=glHashVar.value(mVarName2).trimmed();

                if(mVarValue1.contains(":id"))
                {
                    mStr=glSpace + "scrollId_notExit" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                }
                else
                {
                    mStr=glSpace + "scrollClass_notExit" + "(" + "\"" + mVarValue1 + "\"" + "," + mVarValue2 +");";
                    *stream<<mStr<<endl;
                }
            }
        }
        else
        {
            mStr=glSpace + arg_line;
            *stream<<mStr<<endl;
        }
    }

}

void FileManager::checkStatPoint(const QString &arg_str)
{
    QString mStr=glSpace + "checkStatPoint" + "(" + "\"" + arg_str + "\"" +");";
    *stream<<mStr<<endl;
}
