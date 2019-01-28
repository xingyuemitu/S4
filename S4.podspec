Pod::Spec.new do |s|
  s.name         = "S4"
	s.version      = "2.2.0"
	s.license      = { :type => 'MIT', :file => 'LICENSE.txt' }
	s.summary      = "S4 - Security Library 4 used by 4th A Technologies, LLC."
	s.description  = <<-DESC
S4 is an extensive cross platform library of cryptographic functions that can be called from the C API. It was designed to be portable, such that it can be cross-compiled for different architectures, including OS X, IOS, Linux, Android, and Windows.
                   DESC
	s.homepage     = "https://github.com/4th-ATechnologies/S4"
	s.author    	= "4th-ATechnologies"

	s.ios.deployment_target = '9.2'
	s.osx.deployment_target = '10.10'
#	s.tvos.deployment_target = '9.0'
#	s.watchos.deployment_target = '2.0'

	s.source       = { :git => "https://github.com/4th-ATechnologies/S4.git", :commit => "eba7ce1ea2a7cc3523b5be377e053fc5dc74581b" }
 	s.source_files  = "src/**/*.{h,m}","libs/**/*.{h,m}"
 	s.public_header_files =  "src/main/S4/{s4.h,s4keys.h,s4pubtypes.h,s4scrypto.h,s4bufferutilities.h,s4internal.h}"
 
	s.xcconfig = { "HEADER_SEARCH_PATHS" => "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/ libs/tommath libs/tomcrypt/headers libs/sha3 libs/argon2 libs/tomcrypt/hashes/skein src/main/scripts libs/xxHash libs/yajl/src libs/yajl/src/api libs/jsmn"}
	s.framework  = "S4"
 
  # s.library   = "s4"
  # s.libraries = "iconv", "xml2"

  # s.requires_arc = true

end