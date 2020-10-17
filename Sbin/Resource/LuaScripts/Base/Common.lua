function reloadModule(file)
	package.loaded[file] = nil
	return require(file)
end
