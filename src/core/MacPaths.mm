#include <Foundation/Foundation.h>
#include <string>

std::string applicationSupportPath(){
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *applicationSupportDirectory = [paths firstObject];
	return [applicationSupportDirectory UTF8String];
}

std::string bundleResourcesPath(){
	NSString *bundleResourcesDirectory = [[NSBundle mainBundle] resourcePath];
	return [bundleResourcesDirectory UTF8String];
}
