Conv = Hash[*%w(
青 blue
おばけ object-ghost
メトロノーム filter-swaying
海 filter-bk-wave
フィルタークリア ctrl-filter-clear
白 white
ハート object-heart
上からくりかえしスクロール filter-rolldown
雪 filter-bk-snows
緑 green
もみの木 object-tree
Zoominandout filter-zoom
山 filter-bk-mountain
黄緑 yellowgreen
ヨット object-yacht
ペラペラ [unknown]
草原 filter-bk-grass
黄色 yellow
星 object-star
y軸回転 [unknown]
星空 filter-bk-stars
オレンジ orange
音符 object-note
虹 filter-rainbow
花火 object-fireworks
茶色 brown
靴下 object-socks
スパイラル filter-spiral
赤 red
雪だるま object-snowman
ジャンプ filter-jump
桜吹雪 filter-bk-sakura
)]

f = open('block_identifier/default_instructions.hpp', 'w')

DATA.each do |d|
  color, o2, o3, o4, o6 = d.split
  Hash[2, o2, 3, o3, 4, o4, 6, o6].each do |block, v|
    v2 = Conv[v]
    next if v2.nil?
    param = (v2 =~/object/) ? %( { "lifetime", 5 } ) : ''
    f.puts %({ Block{ "#{Conv[color]}", #{block} }, Instruction{ "#{v2}", Params{#{param}} } }, // #{color}[#{block}]:#{v})
  end
end

__END__
青 おばけ メトロノーム 海 フィルタークリア
白 ハート 上からくりかえしスクロール 雪
緑 もみの木 Zoominandout 山
黄緑 ヨット ペラペラ 草原
黄色 星 y軸回転 星空
オレンジ 音符 虹 花火
茶色 靴下 スパイラル
赤 雪だるま ジャンプ 桜吹雪
