#ifndef BDN_IUiProvider_H_
#define BDN_IUiProvider_H_


namespace bdn
{
	class IUiProvider;
}

#include <bdn/IViewCore.h>
#include <bdn/View.h>

namespace bdn
{


/** Provides the actual implementation for user interface components.

	Most of the UI classes (#Window, #Button, etc) need a platform dependent
	"core object" that creates and manages the actual platform dependent
	UI objects on the screen. These core objects are created by the UI provider.

	Custom implementations of a UI provder can be used to provide special UI implementations.
	For example, one could implement a UI provider that draws all UI objects manually
	into a big canvas, instead of using the normal operating system components.

	A common use case for a special provider implementation is for unit testing.
	A special mock provider can provide fake objects that simplify the testing of a user interface
	and make it easier to automate such tests.
*/
class IUiProvider : BDN_IMPLEMENTS IBase
{
public:

	/** Returns the name of the UI provider. This is intended for logging and diagnostics.*/
	virtual String getName() const=0;


	/** Create thes core for the specified UI object.
	
		@param viewTypeName the type of the view. All standard view types start with "bdn."
			(e.g. bdn.SomeViewName). If you create custom view types then you should start
			with some namespace name followed by a dot and the the name of the view type.
			Usually the namespace prefix should be the name of the C++ namespace, if that can
			be considered "sufficiently unique". If there is no C++ namespace or if it does not
			have a unique name then one should use a different unique prefix. Java style prefixes
			based on the project's domain name (e.g. "com.mydomain.MyType") are recommended in this case.
		@param pView a pointer to the view object. The core object should pull all needed data
			for initialization from this view object (e.g. properties like label, visible and the like).

			The view object must have a type that corresponds to the requested viewTypeName. I.e. it must
			be castable to the corresponding C++ type.
			
			IMPORTANT: the core MUST NOT hold a strong reference to the outer view object, otherwise
			a reference cycle and memory leak will occur. Note that it is guaranteed that
			the view object will exist at least as long as the core object, so a strong reference is
			not necessary.
			So you should not store the view pointer in a smart pointer (like P<View>).
			Use weak pointers (like View*) instead.
			*/
	virtual P<IViewCore> createViewCore(const String& viewTypeName, View* pView)=0;

};


/** Returns the default UI provider for the current platform.

	This function is thread-safe.
*/
P<IUiProvider> getDefaultUiProvider();


/** Sets the default UI provider to use.	

	The default UI provider is used for newly created top level windows.
	Child views inherit the UI provider of their parent by default.

	Changing the default UI provider will only affect newly created windows.
	The UI provider of any existing windows is not modified by this.

	This function is thread-safe.
*/
void setDefaultUiProvider(IUiProvider* pProvider);



}

#endif