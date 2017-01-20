# mruby-gr-wavmp3p
WavMp3p library for mruby-arduino environment.
This library is a wrapper library for Gadget Renesas WavMp3p library.
Currently, only supports GR-CITRUS with WA-MIKAN.

## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    # additional configrations for Arduino API
    conf.cc.flags << " -DGRSAKURA -DARDUINO=100 "
    conf.cc.include_paths << ["../gr_common/lib/", "../gr_common", "../gr_common/core","../gr_common/rx63n", "../gr_common/lib/SPI", "../gr_common/lib/Wire", "../gr_common/lib/Servo", "../gr_common/lib/SD" ]
    conf.cxx.flags = conf.cc.flags.dup
    conf.cxx.include_paths = conf.cc.include_paths.dup
    
    # Arduino API
    conf.gem :github => "takjn/mruby-arduino", :branch => "master"

    # WavMp3p library for mruby-arduino environment.
    conf.gem :github => "takjn/mruby-gr-wavmp3p", :branch => "master"

end
```

## example
```mruby
> a=WavMp3p.new(44100)
 => #<WavMp3p:0x12378>
> a.init(5, 4)
 => true
> a.play("music.wav")
 => ""
```

## License
WavMp3p library is released under the GPL2 License:
- see src/WavMp3p/readme_e.txt or src/WavMp3p/readme_e.txt.

## See also
- [mruby-arduino for Gadget Renesas](https://github.com/takjn/mruby-arduino)
- [mirb for Gadget Renesas](https://github.com/takjn/mirb4gr)