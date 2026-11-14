import { createContext, useContext, type RefObject } from 'react'

const MobilePortalContext = createContext<RefObject<HTMLDivElement | null> | null>(null)

export const MobilePortalProvider = MobilePortalContext.Provider

export function useMobilePortalContainer() {
  return useContext(MobilePortalContext)
}
