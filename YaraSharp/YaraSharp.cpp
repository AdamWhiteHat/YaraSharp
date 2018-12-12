#include "Stdafx.h"

//	CMatches
namespace YaraSharp
{
	//	Check list of yara files
	Dictionary<String^, List<String^>^>^ CYaraSharp::CheckYaraRules([Out] List<String^>^ FilePathList, Dictionary<String^, Object^>^ ExternalVariables)
	{
		Dictionary<String^, List<String^>^>^ CompilationErrors = gcnew Dictionary<String^, List<String^>^>();

		//	Iterative check
		for (int i = 0; i < FilePathList->Count; i++)
		{
			CCompiler^ TestCompiler = gcnew CCompiler(ExternalVariables);

			if (TestCompiler->AddFile(FilePathList[i]))
			{
				if (CompilationErrors->ContainsKey(FilePathList[i]))
				{
					CompilationErrors[FilePathList[i]]->AddRange(TestCompiler->GetErrors(false));
				}
				else
				{
					CompilationErrors->Add(FilePathList[i], TestCompiler->GetErrors(false));
				}

				FilePathList->Remove(FilePathList[i--]);
			}
			else if (TestCompiler->GetErrors(true)->Count > 0)
			{
				if (CompilationErrors->ContainsKey(FilePathList[i]))
				{
					CompilationErrors[FilePathList[i]]->AddRange(TestCompiler->GetErrors(true));
				}
				else
				{
					CompilationErrors->Add(FilePathList[i], TestCompiler->GetErrors(true));
				}
			}

			delete TestCompiler;
		}

		return CompilationErrors;
	}
	
	//	Rule compilation
	CRules^ CYaraSharp::CompileFromFiles(List<String^>^ FilePathList, Dictionary<String^, Object^>^ ExternalVariables,
		[Out] Dictionary<String^, List<String^>^>^% CompilationErrors)
	{
		//	TODO: add namespace support

		//	First: check
		CompilationErrors = CheckYaraRules(FilePathList, ExternalVariables);

		//	Second: compile
		CCompiler^ Compiler = gcnew CCompiler(ExternalVariables);
		Compiler->AddFiles(FilePathList);
		CRules^ Result = Compiler->GetRules();
		delete Compiler;

		//	Return compiled rules
		return Result;
	}

	//	Scanning region
	List<CMatches^>^ CYaraSharp::ScanFile(String^ Path, CRules^ Rules, Dictionary<String^, Object^>^ ExternalVariables, int Timeout)
	{
		CScanner^ FScanner = gcnew CScanner(Rules, ExternalVariables);

		List<CMatches^>^ Results = FScanner->ScanFile(Path);
		delete FScanner;

		return Results;
	}
	//	(not yet tested)
	List<CMatches^>^ CYaraSharp::ScanProcess(int PID, CRules^ Rules, Dictionary<String^, Object^>^ ExternalVariables, int Timeout)
	{
		CScanner^ PScanner = gcnew CScanner(Rules, ExternalVariables);

		List<CMatches^>^ Results = PScanner->ScanProcess(PID);
		delete PScanner;

		return Results;
	}
	//	(not yet tested)
	List<CMatches^>^ CYaraSharp::ScanMemory(uint8_t* Buffer, int Length, CRules^ Rules, Dictionary<String^, Object^>^ ExternalVariables, int Timeout)
	{
		CScanner^ MScanner = gcnew CScanner(Rules, ExternalVariables);

		List<CMatches^>^ Results = MScanner->ScanMemory(Buffer, Length);
		delete MScanner;

		return Results;
	}
	//	(not yet tested)
	List<CMatches^>^ CYaraSharp::ScanMemory(array<uint8_t>^ Buffer, CRules^ Rules, Dictionary<String^, Object^>^ ExternalVariables, int Timeout)
	{
		if (Buffer == nullptr || Buffer->Length == 0)
			return gcnew List<CMatches^>();
		else
		{
			pin_ptr<uint8_t> BufferPointer = &Buffer[0];
			return ScanMemory(BufferPointer, Buffer->Length, Rules, ExternalVariables, Timeout);
		}
	}

	Version^ CYaraSharp::GetVersion()
	{
		return Assembly::GetExecutingAssembly()->GetName()->Version;
	}
}