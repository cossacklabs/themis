#!/usr/bin/env ruby
#
# Update BoringSSL source references in "Themis.xcodeproj" from boringssl.gypi.
#
# Usage:
#
#     third_party/boringssl/update_xcodeproj.rb
#
# Run from Themis source tree root.

begin
  require 'json/next'
  require 'xcodeproj'
rescue LoadError => e
  abort "Missing '#{e.path}' module.\n" \
        "Run \"gem install #{e.path.sub('/', '-')}\" to install it."
end

# Open Themis project file and BoringSSL source list.
# "*.gypi" files are not strictly JSON so we need JSONX parser to handle them.
$project = Xcodeproj::Project.open('Themis.xcodeproj')
$gypi = JSONX.parse(File.read('third_party/boringssl/boringssl.gypi'))

# Locate the "BoringSSL" group in the project. It has been created manually.
$boringssl_group = $project.main_group['BoringSSL']
unless $boringssl_group
  abort 'Missing "BoringSSL" group in Themis.xcodeproj.'
end

# Update source file lists in BoringSSL subgroups.
def update_sources(gypi_name, group_name)
  # This is the list of files that should be in a subgroup. The paths are
  # specified relative to "third_party/boringssl", add this prefix to make
  # the paths relative to the source tree root.
  gypi_files = $gypi['variables'][gypi_name]
  gypi_files = Set.new(gypi_files) { |p| 'third_party/boringssl/' + p }

  # Find BoringSSL subgroup to put the files into. Create it if necessary.
  group = $boringssl_group.find_subpath(group_name, should_create: true)

  # Remove all files present in the subgroup but not present in GYP file list.
  # That is, remove the files which have been removed since the last update.
  group.files.each do |file|
    unless gypi_files.include? file.path
      file.remove_from_project
    end
  end

  # Add all files present in GYP file list but not present in the subgroup.
  # That is, add the files which have been added since the last update.
  group_files = Set.new(group.files) { |f| f.path }
  gypi_files.each do |file|
    unless group_files.include? file
      group.new_file(file, :project)
    end
  end
end

update_sources('boringssl_ssl_sources',         'libssl')
update_sources('boringssl_crypto_sources',      'libcrypto')
update_sources('boringssl_ios_aarch64_sources', 'iOS/ARM64')
update_sources('boringssl_ios_arm_sources',     'iOS/ARMv7')
update_sources('boringssl_mac_x86_sources',     'macOS/x86')
update_sources('boringssl_mac_x86_64_sources',  'macOS/x86_64')
update_sources('boringssl_ios_aarch64_sources', 'macOS/ARM64') # (yes, iOS)

# Locate "BoringSSL (iOS)" and "BoringSSL (macOS)" targets in the project.
# They have been created manually. Add all relevant files to the targets.
# (Removed files have been dropped when they were removed from the project.)
def update_references(target_name, subgroups)
  target = $project.targets.find { |t| t.name == target_name }
  unless target
    abort "Missing \"#{target_name}\" target in Themis.xcodeproj"
  end

  sources = target.source_build_phase
  subgroups.each do |subgroup|
    $boringssl_group[subgroup].files.each do |file|
      next if file.path.end_with? '.h' # Don't need to build headers
      sources.add_file_reference(file, avoid_duplicates: true)
    end
  end
end

update_references('BoringSSL (iOS)',   ['libssl', 'libcrypto', 'iOS/ARM64', 'iOS/ARMv7'])
update_references('BoringSSL (macOS)', ['libssl', 'libcrypto', 'macOS/x86', 'macOS/x86_64', 'macOS/ARM64'])

# Write changes to disk.
$project.save if $project.dirty?
