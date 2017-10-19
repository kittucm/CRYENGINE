// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "SystemAssets.h"

#include <CrySandbox/CrySignal.h>
#include <array>

#include <QVariant>

namespace ACE
{
class CImplItem;

static QVariant GetHeaderData(int const section, Qt::Orientation const orientation, int const role)
{
	if (orientation != Qt::Horizontal)
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		return "Name";
	}

	return QVariant();
}

class CSystemAssetsManager
{
	friend class IUndoControlOperation;
	friend class CUndoControlModified;
	friend class CSystemControl;

public:

	CSystemAssetsManager();
	~CSystemAssetsManager();

	void Initialize();
	void Clear();

	// Libraries
	CSystemLibrary* CreateLibrary(string const& name);
	CSystemLibrary* GetLibrary(size_t const index) const { return m_audioLibraries[index]; }
	size_t          GetLibraryCount() const              { return m_audioLibraries.size(); }

	//
	CSystemAsset*   CreateFolder(string const& name, CSystemAsset* const pParent = nullptr);
	CSystemControl* CreateControl(string const& controlName, ESystemItemType const type, CSystemAsset* const pParent = nullptr);
	void            DeleteItem(CSystemAsset* const pItem);

	CSystemControl* GetControlByID(CID const id) const;
	CSystemControl* FindControl(string const& controlName, ESystemItemType const type, CSystemAsset* const pParent = nullptr) const;

	using Controls = std::vector<CSystemControl*>;
	Controls const& GetControls() const { return m_controls; }

	// Scope
	void       ClearScopes();
	void       AddScope(string const& name, bool const isLocalOnly = false);
	bool       ScopeExists(string const& name) const;
	Scope      GetScope(string const& name) const;
	SScopeInfo GetScopeInfo(Scope const id) const;
	void       GetScopeInfoList(ScopeInfoList& scopeList) const;

	// Helper functions
	void ClearAllConnections();
	void ReloadAllConnections();
	void MoveItems(CSystemAsset* const pParent, std::vector<CSystemAsset*> const& items);
	void CreateAndConnectImplItems(CImplItem* const pImplItem, CSystemAsset* const pParent);

	bool IsTypeDirty(ESystemItemType const type) const;
	bool IsDirty() const;
	void ClearDirtyFlags();
	bool IsLoading() const { return m_isLoading; }

	void SetAssetModified(CSystemAsset* const pAsset, bool const isModified);

	void UpdateAllConnectionStates();
	void UpdateLibraryConnectionStates(CSystemAsset* pAsset);
	void UpdateAssetConnectionStates(CSystemAsset* const pAsset);

	std::vector<string> GetModifiedLibraries() const { return m_modifiedLibraries; }

	// Dirty flags signal
	CCrySignal<void(bool)> signalIsDirty;

	// Library signals
	CCrySignal<void()>                signalLibraryAboutToBeAdded;
	CCrySignal<void(CSystemLibrary*)> signalLibraryAdded;
	CCrySignal<void(CSystemLibrary*)> signalLibraryAboutToBeRemoved;
	CCrySignal<void()>                signalLibraryRemoved;

	// Items signals
	CCrySignal<void(CSystemAsset*)>                signalItemAboutToBeAdded;
	CCrySignal<void(CSystemAsset*)>                signalItemAdded;
	CCrySignal<void(CSystemAsset*)>                signalItemAboutToBeRemoved;
	CCrySignal<void(CSystemAsset*, CSystemAsset*)> signalItemRemoved;

	CCrySignal<void(CSystemControl*)>              signalControlModified;
	CCrySignal<void(CSystemControl*)>              signalConnectionAdded;
	CCrySignal<void(CSystemControl*)>              signalConnectionRemoved;

private:

	CSystemAsset* CreateAndConnectImplItemsRecursively(CImplItem* const pImplItem, CSystemAsset* const pParent);
	void         OnControlAboutToBeModified(CSystemControl* const pControl);
	void         OnControlModified(CSystemControl* const pControl);
	void         OnConnectionAdded(CSystemControl* const pControl, CImplItem* const pImplControl);
	void         OnConnectionRemoved(CSystemControl* const pControl, CImplItem* const pImplControl);

	CID          GenerateUniqueId() { return m_nextId++; }

	std::vector<CSystemLibrary*> m_audioLibraries;

	static CID                   m_nextId;
	Controls                     m_controls;
	std::map<Scope, SScopeInfo>  m_scopeMap;
	std::vector<ESystemItemType> m_modifiedTypes;
	std::vector<string>          m_modifiedLibraries;
	bool                         m_isLoading = false;
};

namespace Utils
{
Scope         GetGlobalScope();
string        GenerateUniqueName(string const& name, ESystemItemType const type, CSystemAsset* const pParent);
string        GenerateUniqueLibraryName(string const& name, CSystemAssetsManager const& assetManager);
string        GenerateUniqueControlName(string const& name, ESystemItemType const type, CSystemAssetsManager const& assetManager);
CSystemAsset* GetParentLibrary(CSystemAsset* pAsset);
void          SelectTopLevelAncestors(std::vector<CSystemAsset*> const& source, std::vector<CSystemAsset*>& dest);
string const& GetAssetFolder();
} // namespace Utils
} // namespace ACE
