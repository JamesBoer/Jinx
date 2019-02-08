
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

include(CSharpUtilities)

project(JinxPad LANGUAGES CXX CSharp)

set(
	jinxpad_source
	"Properties/AssemblyInfo.cs"
	"Properties/Resources.Designer.cs"
	"Properties/Resources.resx"
	"Properties/Settings.Designer.cs"
	"Properties/Settings.settings"
	"Resources/Jinx.ico"
	"Resources/Jinx.png"
	"AboutWindow.xaml"
	"AboutWindow.xaml.cs"
	"App.config"
	"App.xaml"
	"App.xaml.cs"
	"MainWindow.xaml"
	"MainWindow.xaml.cs"
)
add_executable(${PROJECT_NAME} WIN32 ${jinxpad_source} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Jinx.ico)
set_property(SOURCE App.xaml PROPERTY VS_XAML_TYPE "ApplicationDefinition")
set_source_files_properties("Resources/Jinx.png" "Resources/Jinx.ico" PROPERTIES VS_TOOL_OVERRIDE "Resource")
csharp_set_xaml_cs_properties(${jinxpad_source})
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
target_link_libraries(${PROJECT_NAME} JinxTools)
