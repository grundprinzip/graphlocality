orig = File.open("facebook/nodes_clustered.txt").readlines.collect {|l| l.split }
#orig = File.open("nodes_t2.txt").readlines.collect {|l| l.split }
idmap = File.open("id_map.txt").readlines.collect {|l| l.split }.inject(Hash.new) {|m, v| m[v[1]]=v[0]; m}


orig.each do |pair|
	puts "#{idmap[pair[0]]} #{idmap[pair[1]]}"
end