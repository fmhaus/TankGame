#pragma once

#include "engine/Types.h"

#include <string>
#include <functional>
#include <memory>

// TODO: implement functionality for async loading

template<typename T>
struct Asset;

/// !!! All AssetRefs need to be destructed BEFORE the Asset
/// Holds a reference to the asset and assures the asset stays loaded as long as the AssetRef instance exists
template<typename T>
struct AssetRef
{
	AssetRef(Asset<T>* asset)
		: asset(asset)
	{
		if (asset)
			asset->inc_ref();
	}

	~AssetRef() {
		if (asset)
			asset->dec_ref();
	}

	AssetRef(const AssetRef& other)
		: asset(other.asset)
	{
		if (asset)
			asset->inc_ref();
	}

	AssetRef& operator=(const AssetRef& other)
	{
		if (this != &other)
		{
			if (this->asset)
				this->asset->dec_ref();

			this->asset = other.asset;
			if (this->asset)
				this->asset->inc_ref();
		}
		return *this;
	}

	AssetRef(AssetRef&& other) noexcept
		: asset(other.asset)
	{
		other.asset = nullptr;
	};

	AssetRef& operator=(AssetRef&& other) noexcept
	{
		if (this != &other)
		{
			if (this->asset)
				this->asset->dec_ref();
			this->asset = other.asset;
			other.asset = nullptr;
		}
		return *this;
	};

	T& get() { return *asset->data.get(); }
	const T& get() const { return *asset->data.get(); }

	explicit operator bool() const { return asset && asset->data; }
	bool operator==(const AssetRef& other) const { return asset == other.asset; }
	bool operator!=(const AssetRef& other) const { return !(*this == other); }


	friend Asset<T>;

private:

	Asset<T>* asset;
};

/// Stores the asset location with a loader function and counts references to it
/// The asset is unloaded if all AssetRef instances are destroyed
/// Also this struct is really chunky (120 bytes) and can still be optimized a lot
template<typename T>
struct Asset : NoCopy
{
	Asset()
		: location(), loader_fn(), data(), ref_count(0)
	{
	}

	Asset(const std::string location, std::function<void(std::unique_ptr<T>& data_ptr, const std::string& location)> loader_fn)
		: location(std::move(location)), loader_fn(loader_fn), data(nullptr), ref_count(0)
	{
	}

	~Asset()
	{
		// Should never destroy asset before all references are destroyed
		assert(this->ref_count == 0);
	}
	
	void set(const std::string location, std::function<void(std::unique_ptr<T>& data_ptr, const std::string& location)> loader_fn)
	{
		assert(this->ref_count == 0); // Should not change asset location/loader while loaded
		this->location = location;
		this->loader_fn = loader_fn;
	}

	/// Gets a AssetRef to the asset and loads it if not already loaded
	AssetRef<T> loaded()
	{
		return AssetRef<T>(this);
	}

	friend AssetRef<T>;

private:

	void dec_ref()
	{
		this->ref_count--;
		if (this->ref_count == 0)
		{
			this->data.reset();
		}
	}

	void inc_ref()
	{
		if (this->ref_count == 0)
		{
			this->loader_fn(this->data, this->location);
		}
		this->ref_count++;
	}

	std::string location;
	std::function<void(std::unique_ptr<T>& data_ptr, const std::string& location)> loader_fn;
	std::unique_ptr<T> data;
	u32 ref_count;
};