
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

include(CSharpUtilities)

project(JinxTools LANGUAGES CXX CSharp)

set(
	jinxtools_source
	"Properties/AssemblyInfo.cs"
	"Editor.xaml"
	"Editor.xaml.cs"
)
add_library(${PROJECT_NAME} SHARED ${jinxtools_source})
csharp_set_xaml_cs_properties(${jinxtools_source})
target_compile_options(${PROJECT_NAME} PRIVATE "/langversion:6")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
	"Microsoft.CSharp"
	"PresentationCore"
	"PresentationFramework"
	"System"
	"System.Core"
	"System.Data"
	"System.Data.DataSetExtensions"
	"System.Drawing"
	"System.Windows.Forms"
	"System.Xaml"
	"System.Xml"
	"System.Xml.Linq"
	"WindowsBase"
	"WindowsFormsIntegration"
)
