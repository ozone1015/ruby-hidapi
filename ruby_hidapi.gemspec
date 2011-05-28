# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "ruby_hidapi/version"

Gem::Specification.new do |s|
  s.name        = "Ruby HIDAPI"
  s.version     = RubyHidapi::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Chase McCarthy"]
  s.email       = ["ozone1015@gmail.com"]
  s.homepage    = ""
  s.summary     = %q{A Ruby extension for HIDAPI}
  s.description = %q{A Ruby extension for HIDAPI}

  s.rubyforge_project = "ruby_hidapi"

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]
end
